import os
import sys
import csv
import time
from datetime import datetime
import serial
import serial.tools.list_ports

from PyQt6 import uic
from PyQt6.QtCore import QThread, pyqtSignal, Qt
from PyQt6.QtGui import QColor
from PyQt6.QtWidgets import (
    QApplication, QMainWindow, QTableWidgetItem, QHeaderView,
    QFileDialog, QMessageBox, QAbstractItemView
)

FAULT_PRESETS = {
    0: "BA AD F0 0D",
    1: "FF FF FF FF",
    2: "00 00 00 00",
    3: "AA 55 AA 55",
    4: "FA CE B0 0C",
}


class SerialBridgeWorker(QThread):
    """Background worker polling serial bridge (0xAA..0x55 frames & ASCII logs)."""
    transaction_signal = pyqtSignal(str, str, str, int, str)
    system_log_signal = pyqtSignal(str)
    connection_signal = pyqtSignal(bool, str)

    def __init__(self, port: str, baudrate: int = 115200):
        super().__init__()
        self.port = port
        self.baudrate = baudrate
        self.running = False
        self.ser = None

    def run(self):
        rx_buffer = bytearray()
        try:
            self.ser = serial.Serial(self.port, self.baudrate, timeout=0.05)
            self.running = True
            self.connection_signal.emit(True, f"Connected to {self.port} ({self.baudrate} bps)")
        except serial.SerialException as e:
            self.connection_signal.emit(False, f"Could not open {self.port}: {e}")
            return

        while self.running:
            try:
                if self.ser and self.ser.in_waiting > 0:
                    rx_buffer.extend(self.ser.read(self.ser.in_waiting))

                    while len(rx_buffer) > 0:
                        sof_idx = rx_buffer.find(b'\xAA')

                        if sof_idx == -1:
                            self._emit_ascii(rx_buffer)
                            rx_buffer.clear()
                            break

                        if sof_idx > 0:
                            self._emit_ascii(rx_buffer[:sof_idx])
                            rx_buffer = rx_buffer[sof_idx:]

                        if len(rx_buffer) < 3:
                            break

                        payload_len = rx_buffer[1]
                        frame_len = payload_len + 3

                        if len(rx_buffer) < frame_len:
                            break

                        if rx_buffer[frame_len - 1] == 0x55:
                            payload = rx_buffer[2: 2 + payload_len]
                            if payload_len >= 1:
                                raw_addr_byte = payload[0]
                                addr_7bit = f"0x{(raw_addr_byte >> 1):02X}"
                                is_read = bool(raw_addr_byte & 0x01)
                                direction = "READ (EMULATOR TX)" if is_read else "WRITE (MASTER TX)"
                                data_bytes = payload[1:]
                                data_hex = " ".join(f"{b:02X}" for b in data_bytes)
                                ts = datetime.now().strftime("%H:%M:%S.%f")[:-3]

                                self.transaction_signal.emit(ts, direction, addr_7bit, len(data_bytes), data_hex)
                            rx_buffer = rx_buffer[frame_len:]
                        else:
                            rx_buffer.pop(0)
                else:
                    time.sleep(0.005)
            except Exception as e:
                self.system_log_signal.emit(f"[ERROR] {e}")
                break

        if self.ser and self.ser.is_open:
            self.ser.close()
        self.connection_signal.emit(False, "Disconnected")

    def _emit_ascii(self, raw_bytes: bytearray):
        try:
            text = raw_bytes.decode("ascii", errors="ignore").strip()
            if text:
                for line in text.splitlines():
                    clean = line.strip()
                    if clean:
                        self.system_log_signal.emit(f"[DEVICE] {clean}")
        except Exception:
            pass

    def send_config_command(self, mode: int, target_addr: int, fault_bytes: list[int]) -> bool:
        if not self.ser or not self.ser.is_open:
            return False
        b = (fault_bytes + [0x00, 0x00, 0x00, 0x00])[:4]
        cmd_frame = bytearray([0xBB, mode & 0xFF, target_addr & 0x7F, b[0], b[1], b[2], b[3], 0x55])
        try:
            self.ser.write(cmd_frame)
            return True
        except Exception as e:
            self.system_log_signal.emit(f"[TX ERROR] {e}")
            return False

    def stop(self):
        self.running = False
        self.wait(1000)


class ProtocolAnalyzerApp(QMainWindow):
    def __init__(self):
        super().__init__()
        ui_path = os.path.join(os.path.dirname(__file__), "analyzer_window.ui")
        uic.loadUi(ui_path, self)

        self.worker = None
        self.paused = False
        self.total_count = 0
        self.write_count = 0
        self.read_count = 0

        self._configure_table()
        self._connect_signals()
        self._refresh_ports()
        self._log_console("[SYSTEM] Application ready.")

    def _configure_table(self):
        header = self.table_transactions.horizontalHeader()
        header.setSectionResizeMode(0, QHeaderView.ResizeMode.Fixed)
        header.resizeSection(0, 60)
        header.setSectionResizeMode(1, QHeaderView.ResizeMode.Fixed)
        header.resizeSection(1, 120)
        header.setSectionResizeMode(2, QHeaderView.ResizeMode.Fixed)
        header.resizeSection(2, 180)
        header.setSectionResizeMode(3, QHeaderView.ResizeMode.Fixed)
        header.resizeSection(3, 110)
        header.setSectionResizeMode(4, QHeaderView.ResizeMode.Fixed)
        header.resizeSection(4, 90)
        header.setSectionResizeMode(5, QHeaderView.ResizeMode.Stretch)

        self.table_transactions.verticalHeader().setVisible(False)
        self.table_transactions.setEditTriggers(QAbstractItemView.EditTrigger.NoEditTriggers)
        self.table_transactions.setSelectionBehavior(QAbstractItemView.SelectionBehavior.SelectRows)

    def _connect_signals(self):
        self.btn_refresh_ports.clicked.connect(self._refresh_ports)
        self.btn_connect.clicked.connect(self._toggle_connection)
        self.btn_apply_config.clicked.connect(self._apply_hardware_config)
        self.btn_reset_config.clicked.connect(self._reset_default_config)
        self.combo_fault_presets.currentIndexChanged.connect(self._on_preset_changed)
        self.combo_filter_addr.currentIndexChanged.connect(self._apply_table_filter)
        self.btn_pause_stream.clicked.connect(self._toggle_pause)
        self.btn_export_csv.clicked.connect(self._export_csv)
        self.btn_clear_log.clicked.connect(self._clear_table)

    def _refresh_ports(self):
        self.combo_port.clear()
        ports = serial.tools.list_ports.comports()
        for p in ports:
            self.combo_port.addItem(f"{p.device}", p.device)
        idx = self.combo_port.findData("COM3")
        if idx >= 0:
            self.combo_port.setCurrentIndex(idx)
        elif self.combo_port.count() == 0:
            self.combo_port.addItem("COM3", "COM3")

    def _toggle_connection(self):
        if self.worker and self.worker.isRunning():
            self.worker.stop()
            self.worker = None
        else:
            port = self.combo_port.currentData() or self.combo_port.currentText().split()[0]
            baud = int(self.combo_baud.currentText())
            self.worker = SerialBridgeWorker(port, baud)
            self.worker.transaction_signal.connect(self._on_transaction_received)
            self.worker.system_log_signal.connect(self._log_console)
            self.worker.connection_signal.connect(self._on_connection_state_changed)
            self.worker.start()

    def _on_connection_state_changed(self, connected: bool, message: str):
        self._log_console(f"[SYSTEM] {message}")
        self.statusbar.showMessage(message)
        if connected:
            self.btn_connect.setText("Disconnect")
            self.btn_apply_config.setEnabled(True)
            self.combo_port.setEnabled(False)
            self.combo_baud.setEnabled(False)
            self.lbl_conn_status.setText(f"● CONNECTED ({self.worker.port})")
            self.lbl_conn_status.setStyleSheet(
                "background-color: #064E3B; color: #34D399; border: 1px solid #10B981; "
                "border-radius: 4px; padding: 0px 12px; font-weight: 600;"
            )
            self._apply_hardware_config()
        else:
            self.btn_connect.setText("Connect")
            self.btn_apply_config.setEnabled(False)
            self.combo_port.setEnabled(True)
            self.combo_baud.setEnabled(True)
            self.lbl_conn_status.setText("● DISCONNECTED")
            self.lbl_conn_status.setStyleSheet(
                "background-color: #3F1D24; color: #F87171; border: 1px solid #EF4444; "
                "border-radius: 4px; padding: 0px 12px; font-weight: 600;"
            )

    def _on_preset_changed(self, index: int):
        if index in FAULT_PRESETS:
            self.input_fault_payload.setText(FAULT_PRESETS[index])

    def _reset_default_config(self):
        self.combo_mode.setCurrentIndex(0)
        self.input_target_addr.setText("0x7E")
        self.combo_fault_presets.setCurrentIndex(0)
        self.input_fault_payload.setText(FAULT_PRESETS[0])
        if self.worker and self.worker.isRunning():
            self._apply_hardware_config()

    def _apply_hardware_config(self):
        if not self.worker or not self.worker.isRunning():
            return
        try:
            mode = self.combo_mode.currentIndex()
            addr_text = self.input_target_addr.text().strip()
            target_addr = int(addr_text, 16)

            hex_tokens = self.input_fault_payload.text().replace(",", " ").split()
            fault_bytes = [int(tok, 16) for tok in hex_tokens[:4]]
            if not fault_bytes:
                raise ValueError("Provide at least 1 hex byte for Fault Payload.")
        except ValueError as e:
            QMessageBox.warning(self, "Invalid Hex Input", f"Please check your Hex values:\n{e}")
            return

        if self.worker.send_config_command(mode, target_addr, fault_bytes):
            formatted_fault = " ".join(f"{b:02X}" for b in fault_bytes)
            self._log_console(
                f"[CONFIG] Sent 0xBB | Mode={mode} | Target=0x{target_addr:02X} | Payload=[{formatted_fault}]"
            )

    def _on_transaction_received(self, ts: str, direction: str, addr_hex: str, length: int, data_hex: str):
        if self.paused:
            return

        self.total_count += 1
        is_read = "READ" in direction
        if is_read:
            self.read_count += 1
        else:
            self.write_count += 1

        self.lbl_total_count.setText(f"Total: {self.total_count}")
        self.lbl_write_count.setText(f"Writes: {self.write_count}")
        self.lbl_read_count.setText(f"Reads: {self.read_count}")

        row = self.table_transactions.rowCount()
        self.table_transactions.insertRow(row)

        row_items = [
            QTableWidgetItem(str(self.total_count)),
            QTableWidgetItem(ts),
            QTableWidgetItem(direction),
            QTableWidgetItem(addr_hex),
            QTableWidgetItem(f"{length} B"),
            QTableWidgetItem(data_hex),
        ]

        fg_dir_color = QColor("#FB923C") if is_read else QColor("#4ADE80")
        addr_color = QColor("#38BDF8") if addr_hex.upper() == "0X18" else QColor("#E879F9")

        for col, item in enumerate(row_items):
            if col == 2:
                item.setForeground(fg_dir_color)
            elif col == 3:
                item.setForeground(addr_color)
            if col < 5:
                item.setTextAlignment(Qt.AlignmentFlag.AlignCenter)
            self.table_transactions.setItem(row, col, item)

        self._filter_single_row(row)

        if self.chk_autoscroll.isChecked():
            self.table_transactions.scrollToBottom()

    def _apply_table_filter(self):
        for r in range(self.table_transactions.rowCount()):
            self._filter_single_row(r)

    def _filter_single_row(self, row: int):
        filter_idx = self.combo_filter_addr.currentIndex()
        if filter_idx == 0:
            self.table_transactions.setRowHidden(row, False)
            return
        addr_item = self.table_transactions.item(row, 3)
        if not addr_item:
            return
        target_prefix = "0x18" if filter_idx == 1 else "0x7E"
        self.table_transactions.setRowHidden(row, not addr_item.text().upper().startswith(target_prefix.upper()))

    def _toggle_pause(self):
        self.paused = not self.paused
        self.btn_pause_stream.setText("Resume" if self.paused else "Pause")
        self._log_console(f"[SYSTEM] Capture {'paused' if self.paused else 'resumed'}.")

    def _clear_table(self):
        self.table_transactions.setRowCount(0)
        self.total_count = 0
        self.write_count = 0
        self.read_count = 0
        self.lbl_total_count.setText("Total: 0")
        self.lbl_write_count.setText("Writes: 0")
        self.lbl_read_count.setText("Reads: 0")

    def _export_csv(self):
        if self.table_transactions.rowCount() == 0:
            QMessageBox.information(self, "Export CSV", "No transactions to export yet.")
            return
        path, _ = QFileDialog.getSaveFileName(
            self, "Export Capture Log", "i2c_capture_log.csv", "CSV Files (*.csv)"
        )
        if not path:
            return
        with open(path, "w", newline="", encoding="utf-8") as f:
            writer = csv.writer(f)
            headers = [self.table_transactions.horizontalHeaderItem(c).text() for c in range(6)]
            writer.writerow(headers)
            for r in range(self.table_transactions.rowCount()):
                writer.writerow([
                    self.table_transactions.item(r, c).text() if self.table_transactions.item(r, c) else ""
                    for c in range(6)
                ])
        self._log_console(f"[SYSTEM] Exported {self.table_transactions.rowCount()} rows to {path}")

    def _log_console(self, msg: str):
        ts = datetime.now().strftime("%H:%M:%S")
        self.txt_console.append(f"[{ts}] {msg}")

    def closeEvent(self, event):
        if self.worker and self.worker.isRunning():
            self.worker.stop()
        event.accept()


if __name__ == "__main__":
    app = QApplication(sys.argv)
    window = ProtocolAnalyzerApp()
    window.show()
    sys.exit(app.exec())