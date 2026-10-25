import os
import sys
import time
import serial
import pytest

# Since this file is in software/tests/, ".." points directly to software/
sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), "..")))
from dashboard import SerialBridgeWorker

COM_PORT = os.environ.get("HIL_COM_PORT", "COM3")
BAUD_RATE = 115200


class HILTestHarness:
    """Wraps the production SerialBridgeWorker from dashboard.py to run live HIL assertions."""

    def __init__(self, port: str, baudrate: int):
        self.worker = SerialBridgeWorker(port, baudrate)
        self.rx_buffer = bytearray()
        self.transactions: list[dict] = []
        self.logs: list[str] = []

        self.worker.transaction_signal.connect(self._on_transaction)
        self.worker.system_log_signal.connect(self._on_log)

        self.worker.ser = serial.Serial(port, baudrate, timeout=0.05)
        time.sleep(0.2)
        self.worker.ser.reset_input_buffer()

    def _on_transaction(self, ts: str, direction: str, addr_hex: str, length: int, data_hex: str):
        self.transactions.append({
            "timestamp": ts,
            "direction": direction,
            "addr": addr_hex.upper(),
            "length": length,
            "data_hex": data_hex.upper(),
        })

    def _on_log(self, msg: str):
        self.logs.append(msg)

    def poll_for(self, duration_sec: float):
        """Polls COM3 for duration_sec and feeds bytes through SerialBridgeWorker._process_rx_buffer()."""
        end_time = time.time() + duration_sec
        while time.time() < end_time:
            if self.worker.ser.in_waiting > 0:
                self.rx_buffer.extend(self.worker.ser.read(self.worker.ser.in_waiting))
                self.worker._process_rx_buffer(self.rx_buffer)
            time.sleep(0.01)

    def configure_and_sync(self, mode: int, target_addr: int, fault_bytes: list[int], timeout: float = 2.0) -> str:
        """
        Sends 0xBB config frame via SerialBridgeWorker, waits for [CFG-ACK] from NXP MCU,
        and flushes pre-config transactions to prevent stale buffer race conditions.
        """
        self.logs.clear()
        self.worker.ser.reset_input_buffer()
        self.rx_buffer.clear()

        sent_ok = self.worker.send_config_command(mode, target_addr, fault_bytes)
        assert sent_ok, "Failed to write 0xBB command to serial port"

        end_time = time.time() + timeout
        ack_line = None
        while time.time() < end_time:
            if self.worker.ser.in_waiting > 0:
                self.rx_buffer.extend(self.worker.ser.read(self.worker.ser.in_waiting))
                self.worker._process_rx_buffer(self.rx_buffer)
                for log in self.logs:
                    if "[CFG-ACK]" in log:
                        ack_line = log
                        break
                if ack_line:
                    break
            time.sleep(0.01)

        assert ack_line is not None, f"Timed out waiting for [CFG-ACK] from NXP MCU. Logs: {self.logs}"
        self.transactions.clear()
        return ack_line

    def close(self):
        if self.worker.ser and self.worker.ser.is_open:
            self.worker.ser.close()


@pytest.fixture(scope="module")
def hil():
    """Opens COM3 once for the HIL module and restores default Analyzer state upon completion."""
    try:
        harness = HILTestHarness(COM_PORT, BAUD_RATE)
    except serial.SerialException as e:
        pytest.fail(
            f"Could not open {COM_PORT}. Ensure NXP J10 USB is connected and dashboard.py is closed. Error: {e}"
        )
    yield harness
    try:
        harness.configure_and_sync(mode=0, target_addr=0x7E, fault_bytes=[0xBA, 0xAD, 0xF0, 0x0D])
    except Exception:
        pass
    harness.close()


def test_hil01_uart_command_handshake(hil: HILTestHarness):
    """HIL-01: Verify bi-directional 0xBB command execution and [CFG-ACK] response from NXP MCU."""
    ack = hil.configure_and_sync(mode=0, target_addr=0x7E, fault_bytes=[0xBA, 0xAD, 0xF0, 0x0D])
    assert "Mode=ANALYZER(0x18-0x7E)" in ack
    assert "Addr=0x7E" in ack
    assert "FaultPayload=BA AD F0 0D" in ack


def test_hil02_analyzer_multi_address_capture(hil: HILTestHarness):
    """
    HIL-02: Verify Mode 0 passively captures real FXLS8974CFR3 accelerometer (0x18) traffic
    alongside target emulator (0x7E) Master Writes over physical jumper wires.
    """
    hil.configure_and_sync(mode=0, target_addr=0x7E, fault_bytes=[0xBA, 0xAD, 0xF0, 0x0D])
    hil.poll_for(5.5)

    accel_writes = [
        tx for tx in hil.transactions
        if tx["addr"] == "0X18" and tx["direction"] == "WRITE (MASTER TX)"
    ]
    emul_writes = [
        tx for tx in hil.transactions
        if tx["addr"] == "0X7E" and tx["direction"] == "WRITE (MASTER TX)"
    ]

    assert len(accel_writes) >= 1, f"Missing 0x18 accelerometer write. Captured: {hil.transactions}"
    assert accel_writes[0]["data_hex"] == "13"
    assert accel_writes[0]["length"] == 1

    assert len(emul_writes) >= 1, f"Missing 0x7E master write. Captured: {hil.transactions}"
    assert emul_writes[0]["data_hex"] == "DE AD"
    assert emul_writes[0]["length"] == 2


@pytest.mark.parametrize(
    "fault_bytes,expected_2byte_hex",
    [
        ([0xBA, 0xAD, 0xF0, 0x0D], "BA AD"),
        ([0xFF, 0xFF, 0xFF, 0xFF], "FF FF"),
        ([0x00, 0x00, 0x00, 0x00], "00 00"),
        ([0xAA, 0x55, 0xAA, 0x55], "AA 55"),
    ],
)
def test_hil03_dynamic_fault_injection_sweep(hil: HILTestHarness, fault_bytes: list[int], expected_2byte_hex: str):
    """
    HIL-03: Sweep all 4 fault presets via 0xBB and verify the NXP Slave Transmit ISR
    physically drives the exact corrupted payload onto the I2C bus during Pico Master Reads.
    """
    ack = hil.configure_and_sync(mode=0, target_addr=0x7E, fault_bytes=fault_bytes)
    expected_ack_payload = " ".join(f"{b:02X}" for b in fault_bytes)
    assert f"FaultPayload={expected_ack_payload}" in ack

    end_time = time.time() + 5.5
    read_tx = None
    while time.time() < end_time:
        hil.poll_for(0.2)
        reads = [
            tx for tx in hil.transactions
            if tx["addr"] == "0X7E" and tx["direction"] == "READ (EMULATOR TX)"
        ]
        if reads:
            read_tx = reads[-1]
            break

    assert read_tx is not None, f"No 0x7E READ transaction captured for fault pattern {expected_2byte_hex}"
    assert read_tx["length"] == 2
    assert read_tx["data_hex"] == expected_2byte_hex


def test_hil04_emulator_mode_hardware_isolation(hil: HILTestHarness):
    """
    HIL-04: Switch LPI2C2 hardware to Mode 1 (kLPI2C_MatchAddress0 on 0x7E only)
    and verify that 0x18 accelerometer traffic is filtered out in silicon while 0x7E still works.
    """
    ack = hil.configure_and_sync(mode=1, target_addr=0x7E, fault_bytes=[0xCA, 0xFE, 0x01, 0x02])
    assert "Mode=EMULATOR" in ack

    hil.poll_for(5.5)

    accel_packets = [tx for tx in hil.transactions if tx["addr"] == "0X18"]
    emul_packets = [tx for tx in hil.transactions if tx["addr"] == "0X7E"]

    assert len(accel_packets) == 0, f"Mode 1 failed to filter out 0x18 sensor traffic! Got: {accel_packets}"
    assert len(emul_packets) >= 2, f"Expected 0x7E Write & Read in Emulator Mode, got: {emul_packets}"


def test_hil05_dynamic_address_reassignment_filtering(hil: HILTestHarness):
    """
    HIL-05: Configure Mode 1 with an unused target address (0x55). Verify that the LPI2C2
    address0 register updates in silicon and ignores both 0x18 and 0x7E traffic.
    """
    ack = hil.configure_and_sync(mode=1, target_addr=0x55, fault_bytes=[0x11, 0x22, 0x33, 0x44])
    assert "Mode=EMULATOR" in ack
    assert "Addr=0x55" in ack

    # Poll for a full 4.5s Pico cycle; since Pico only addresses 0x18 and 0x7E, zero packets should match
    hil.poll_for(4.5)
    assert len(hil.transactions) == 0, (
        f"Expected 0 matched packets when emulating unused address 0x55, but got: {hil.transactions}"
    )


def test_hil06_continuous_bus_stress_stability(hil: HILTestHarness):
    """
    HIL-06: Restore Mode 0 (Analyzer) and run a continuous 12.5-second capture (~3 full Pico cycles).
    Assert >= 9 valid transactions captured with 100% payload integrity.
    """
    hil.configure_and_sync(mode=0, target_addr=0x7E, fault_bytes=[0xBA, 0xAD, 0xF0, 0x0D])
    hil.poll_for(12.5)

    assert len(hil.transactions) >= 9, (
        f"Expected at least 9 transactions over 12.5s stress window, captured {len(hil.transactions)}"
    )

    for tx in hil.transactions:
        if tx["addr"] == "0X18":
            assert tx["direction"] == "WRITE (MASTER TX)"
            assert tx["data_hex"] == "13"
        elif tx["addr"] == "0X7E" and tx["direction"] == "WRITE (MASTER TX)":
            assert tx["data_hex"] == "DE AD"
        elif tx["addr"] == "0X7E" and tx["direction"] == "READ (EMULATOR TX)":
            assert tx["data_hex"] == "BA AD"