import os
import sys
import pytest
from PyQt6.QtWidgets import QApplication

# Since this file is in software/tests/, ".." points directly to software/
sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), "..")))

from dashboard import (
    SerialBridgeWorker,
    ProtocolAnalyzerApp,
    FAULT_PRESETS,
    get_resource_path,
)

# Shared headless QApplication instance for testing GUI logic without opening windows
_qapp = QApplication.instance() or QApplication(sys.argv)


# =====================================================================
# PART A: BACKEND SERIAL WORKER & PROTOCOL FRAMING UNIT TESTS (UT-01..06)
# =====================================================================

def test_ut01_ui_resource_exists():
    """UT-01: Verify that get_resource_path locates analyzer_window.ui on disk."""
    ui_path = get_resource_path("analyzer_window.ui")
    assert os.path.exists(ui_path), f"Missing UI file at {ui_path}"


def test_ut02_build_config_frame_structure_and_padding():
    """UT-02: Verify 0xBB..0x55 8-byte command construction, 7-bit address masking, and 0x00 padding."""
    frame = SerialBridgeWorker.build_config_frame(mode=0, target_addr=0x7E, fault_bytes=[0xBA, 0xAD])
    assert len(frame) == 8
    assert list(frame) == [0xBB, 0x00, 0x7E, 0xBA, 0xAD, 0x00, 0x00, 0x55]


def test_ut03_worker_decodes_master_write_signal():
    """UT-03: Feed a raw 0x18 Master Write frame (0x30) into SerialBridgeWorker and verify its Qt signal."""
    worker = SerialBridgeWorker()
    captured = []
    worker.transaction_signal.connect(
        lambda ts, direction, addr, length, data_hex: captured.append((direction, addr, length, data_hex))
    )

    rx_buf = bytearray([0xAA, 0x02, 0x30, 0x13, 0x55])  # 0x30 >> 1 = 0x18, bit0 = 0 (Write)
    worker._process_rx_buffer(rx_buf)

    assert len(captured) == 1
    assert captured[0] == ("WRITE (MASTER TX)", "0x18", 1, "13")
    assert len(rx_buf) == 0


def test_ut04_worker_decodes_emulator_read_signal():
    """UT-04: Feed a raw 0x7E Emulator Read frame (0xFD) into SerialBridgeWorker and verify its Qt signal."""
    worker = SerialBridgeWorker()
    captured = []
    worker.transaction_signal.connect(
        lambda ts, direction, addr, length, data_hex: captured.append((direction, addr, length, data_hex))
    )

    rx_buf = bytearray([0xAA, 0x03, 0xFD, 0xBA, 0xAD, 0x55])  # 0xFD >> 1 = 0x7E, bit0 = 1 (Read)
    worker._process_rx_buffer(rx_buf)

    assert len(captured) == 1
    assert captured[0] == ("READ (EMULATOR TX)", "0x7E", 2, "BA AD")


def test_ut05_worker_separates_ascii_logs_from_binary_frames():
    """UT-05: Verify mixed ASCII [CFG-ACK] strings and binary I2C packets are separated cleanly."""
    worker = SerialBridgeWorker()
    captured_tx = []
    captured_logs = []

    worker.transaction_signal.connect(lambda ts, d, a, l, h: captured_tx.append((a, h)))
    worker.system_log_signal.connect(lambda msg: captured_logs.append(msg))

    stream = bytearray(b"\r\n[CFG-ACK] Mode=ANALYZER(0x18-0x7E)\r\n") + bytearray([0xAA, 0x03, 0xFC, 0xDE, 0xAD, 0x55])
    worker._process_rx_buffer(stream)

    assert len(captured_logs) == 1
    assert "[CFG-ACK]" in captured_logs[0]
    assert len(captured_tx) == 1
    assert captured_tx[0] == ("0x7E", "DE AD")


def test_ut06_worker_recovers_from_corrupted_header_and_fragmented_chunks():
    """UT-06: Verify parser drops false 0xAA noise and waits for split UART chunks to complete."""
    worker = SerialBridgeWorker()
    captured_tx = []
    worker.transaction_signal.connect(lambda ts, d, a, l, h: captured_tx.append((a, h)))

    # Bogus 0xAA byte (no 0x55 at end) followed by first 3 bytes of a valid frame
    buf = bytearray([0xAA, 0x01, 0x30, 0x99, 0xAA, 0x02, 0x30])
    worker._process_rx_buffer(buf)

    assert len(captured_tx) == 0
    assert list(buf) == [0xAA, 0x02, 0x30]  # Partial valid frame preserved in buffer

    # Append remaining 2 bytes of the valid frame
    buf.extend([0x13, 0x55])
    worker._process_rx_buffer(buf)

    assert len(captured_tx) == 1
    assert captured_tx[0] == ("0x18", "13")
    assert len(buf) == 0


# =====================================================================
# PART B: HEADLESS PYQT6 GUI CONTROLLER LOGIC TESTS (UT-07..10)
# =====================================================================

def test_ut07_gui_table_insertion_and_telemetry_counters():
    """UT-07: Verify ProtocolAnalyzerApp updates counters and table rows when transactions arrive."""
    win = ProtocolAnalyzerApp()
    win._clear_table()

    win._on_transaction_received("12:00:01.000", "WRITE (MASTER TX)", "0x18", 1, "13")
    win._on_transaction_received("12:00:02.000", "WRITE (MASTER TX)", "0x7E", 2, "DE AD")
    win._on_transaction_received("12:00:03.000", "READ (EMULATOR TX)", "0x7E", 2, "BA AD")

    assert win.total_count == 3
    assert win.write_count == 2
    assert win.read_count == 1
    assert win.table_transactions.rowCount() == 3
    assert win.lbl_total_count.text() == "Total: 3"
    assert win.table_transactions.item(2, 5).text() == "BA AD"


def test_ut08_gui_address_filtering_logic():
    """UT-08: Verify combo_filter_addr hides/shows 0x18 and 0x7E rows accurately."""
    win = ProtocolAnalyzerApp()
    win._clear_table()

    win._on_transaction_received("12:00:01.000", "WRITE (MASTER TX)", "0x18", 1, "13")
    win._on_transaction_received("12:00:02.000", "READ (EMULATOR TX)", "0x7E", 2, "BA AD")

    # Filter 1: Show only 0x18 (Sensor)
    win.combo_filter_addr.setCurrentIndex(1)
    assert not win.table_transactions.isRowHidden(0)
    assert win.table_transactions.isRowHidden(1)

    # Filter 2: Show only 0x7E (Emulator)
    win.combo_filter_addr.setCurrentIndex(2)
    assert win.table_transactions.isRowHidden(0)
    assert not win.table_transactions.isRowHidden(1)

    # Filter 0: Show All Addresses
    win.combo_filter_addr.setCurrentIndex(0)
    assert not win.table_transactions.isRowHidden(0)
    assert not win.table_transactions.isRowHidden(1)


def test_ut09_gui_pause_resume_and_clear_controls():
    """UT-09: Verify Pause freezes table updates and Clear resets all counters to zero."""
    win = ProtocolAnalyzerApp()
    win._clear_table()

    win._toggle_pause()  # Pause active
    assert win.paused is True
    win._on_transaction_received("12:00:01.000", "WRITE (MASTER TX)", "0x18", 1, "13")
    assert win.table_transactions.rowCount() == 0  # Ignored while paused

    win._toggle_pause()  # Resume active
    assert win.paused is False
    win._on_transaction_received("12:00:02.000", "WRITE (MASTER TX)", "0x18", 1, "13")
    assert win.table_transactions.rowCount() == 1

    win._clear_table()
    assert win.table_transactions.rowCount() == 0
    assert win.total_count == 0


def test_ut10_gui_fault_presets_and_reset_default():
    """UT-10: Verify selecting fault presets updates input_fault_payload and Reset Default restores baseline."""
    win = ProtocolAnalyzerApp()

    win.combo_mode.setCurrentIndex(1)
    win.input_target_addr.setText("0x55")
    win.combo_fault_presets.setCurrentIndex(1)  # Stuck High (FF FF FF FF)
    assert win.input_fault_payload.text() == FAULT_PRESETS[1]

    win._reset_default_config()
    assert win.combo_mode.currentIndex() == 0
    assert win.input_target_addr.text() == "0x7E"
    assert win.input_fault_payload.text() == "BA AD F0 0D"