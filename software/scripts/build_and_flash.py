import argparse
import os
import shutil
import subprocess
import sys
import time
import serial

SOFTWARE_DIR = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
PROJECT_ROOT = os.path.abspath(os.path.join(SOFTWARE_DIR, ".."))
sys.path.insert(0, SOFTWARE_DIR)

from dashboard import SerialBridgeWorker

# === EXACT TOOLCHAIN & PROJECT PATHS ON THIS MACHINE ===
ARMGCC_ROOT = r"C:\Users\ADARSH\.mcuxpressotools\arm-gnu-toolchain-14.2.rel1-mingw-w64-x86_64-arm-none-eabi"
NINJA_DIR = r"C:\Users\ADARSH\.mcuxpressotools\ninja-1.13.2"
LINKSERVER_EXE = r"C:\nxp\LinkServer_26.6.137\LinkServer.exe"

# Canonical CMake build directory configured with MCUXpresso SDK paths
CANONICAL_FW_DIR = r"C:\_git\_mcx\protocol_analyzer\firmware\i2c_analyzer_firmware"
FW_BUILD_DIR = os.path.join(CANONICAL_FW_DIR, "debug")
ELF_PATH = os.path.join(FW_BUILD_DIR, "i2c_analyzer_firmware.elf")

DEFAULT_DEVICE = "MCXN236:FRDM-MCXN236"
COM_PORT = os.environ.get("HIL_COM_PORT", "COM3")
BAUD_RATE = 115200


def setup_toolchain_env() -> dict:
    """Injects NXP ARM GCC and Ninja paths into the environment for headless CMake builds."""
    env = os.environ.copy()
    env["ARMGCC_DIR"] = ARMGCC_ROOT.replace("\\", "/")

    extra_paths = [
        os.path.join(ARMGCC_ROOT, "bin"),
        NINJA_DIR,
        os.path.dirname(LINKSERVER_EXE),
    ]
    env["PATH"] = os.pathsep.join(extra_paths) + os.pathsep + env.get("PATH", "")
    return env


def sync_runner_firmware_sources() -> None:
    """If running inside a GitHub Actions runner workspace, syncs .c/.h edits to CANONICAL_FW_DIR."""
    workspace_fw_dir = os.path.join(PROJECT_ROOT, "firmware", "i2c_analyzer_firmware")
    if os.path.normcase(workspace_fw_dir) != os.path.normcase(CANONICAL_FW_DIR) and os.path.isdir(workspace_fw_dir):
        for fname in os.listdir(workspace_fw_dir):
            if fname.endswith((".c", ".h")):
                src = os.path.join(workspace_fw_dir, fname)
                dst = os.path.join(CANONICAL_FW_DIR, fname)
                shutil.copy2(src, dst)

def has_firmware_changed() -> bool:
    """Checks Git working tree and last commit to see if C firmware source files changed."""
    def _is_fw_source(path_str: str) -> bool:
        norm = path_str.replace("\\", "/")
        if "firmware/i2c_analyzer_firmware/" not in norm:
            return False
        if "/debug/" in norm:
            return False
        return norm.endswith((".c", ".h", "CMakeLists.txt"))

    try:
        status_out = subprocess.check_output(
            ["git", "status", "--porcelain", "firmware/i2c_analyzer_firmware"],
            cwd=PROJECT_ROOT,
            text=True,
            stderr=subprocess.DEVNULL,
        ).strip()
        for line in status_out.splitlines():
            if _is_fw_source(line):
                print(f"[CI-GIT] Detected uncommitted C source change: {line.strip()}")
                return True

        diff_out = subprocess.check_output(
            ["git", "diff", "--name-only", "HEAD~1", "HEAD"],
            cwd=PROJECT_ROOT,
            text=True,
            stderr=subprocess.DEVNULL,
        ).strip()
        for line in diff_out.splitlines():
            if _is_fw_source(line):
                print(f"[CI-GIT] Detected C firmware change in latest commit: {line.strip()}")
                return True
        return False
    except Exception:
        return True


def build_firmware(env: dict) -> None:
    """Runs headless incremental CMake/Ninja build and verifies the .elf binary output."""
    sync_runner_firmware_sources()
    print(f"\n[STAGE 2A] Building Embedded C Firmware in: {FW_BUILD_DIR}")
    if not os.path.isdir(FW_BUILD_DIR):
        raise FileNotFoundError(f"Build directory not found: {FW_BUILD_DIR}")

    cmd = ["cmake", "--build", FW_BUILD_DIR]
    result = subprocess.run(cmd, env=env, text=True)
    if result.returncode != 0:
        raise RuntimeError(f"CMake firmware build failed with exit code {result.returncode}")

    if not os.path.isfile(ELF_PATH):
        raise FileNotFoundError(f"Expected compiled ELF binary was not found at: {ELF_PATH}")

    size_kb = os.path.getsize(ELF_PATH) / 1024.0
    print(f"[STAGE 2A] SUCCESS: Firmware compiled -> {ELF_PATH} ({size_kb:.1f} KB)")


def detect_linkserver_device() -> str:
    """Queries LinkServer for the exact MCXN236 device identifier string."""
    try:
        out = subprocess.check_output(
            [LINKSERVER_EXE, "devices", "--filter", "MCXN236"],
            text=True,
            stderr=subprocess.STDOUT,
        )
        for line in out.splitlines():
            token = line.strip().split()[0] if line.strip() else ""
            if "MCXN236" in token and ":" in token:
                return token
    except Exception:
        pass
    return DEFAULT_DEVICE


def flash_firmware(device_id: str) -> None:
    """Flashes the compiled .elf onto the NXP FRDM-MCXN236 over USB J10 via LinkServer CLI."""
    print(f"\n[STAGE 2B] Flashing {os.path.basename(ELF_PATH)} to {device_id} via NXP LinkServer...")
    if not os.path.isfile(LINKSERVER_EXE):
        raise FileNotFoundError(f"LinkServer executable not found at: {LINKSERVER_EXE}")

    cmd = [LINKSERVER_EXE, "flash", device_id, "load", ELF_PATH]
    result = subprocess.run(cmd, text=True)
    if result.returncode != 0:
        raise RuntimeError(
            f"LinkServer flashing failed (exit code {result.returncode}). "
            "Ensure USB cable is plugged into J10 and no debugger session is active in VS Code."
        )
    print("[STAGE 2B] SUCCESS: Flash programming complete. Waiting 1.5s for MCU reboot...")
    time.sleep(1.5)


def verify_mcu_boot_and_handshake(port: str, baudrate: int) -> None:
    """Post-flash smoke test: opens COM3, sends 0xBB ping, and asserts [CFG-ACK] response."""
    print(f"\n[STAGE 2C] Running Post-Flash Smoke Test on {port} ({baudrate} bps)...")
    worker = SerialBridgeWorker(port, baudrate)
    rx_buffer = bytearray()
    logs: list[str] = []
    worker.system_log_signal.connect(lambda msg: logs.append(msg))

    try:
        worker.ser = serial.Serial(port, baudrate, timeout=0.05)
    except serial.SerialException as e:
        raise RuntimeError(f"Post-flash smoke test could not open {port}: {e}")

    try:
        time.sleep(0.2)
        worker.ser.reset_input_buffer()
        sent = worker.send_config_command(mode=0, target_addr=0x7E, fault_bytes=[0xBA, 0xAD, 0xF0, 0x0D])
        if not sent:
            raise RuntimeError("Failed to transmit 0xBB handshake frame over UART.")

        end_time = time.time() + 2.5
        ack_received = None
        while time.time() < end_time:
            if worker.ser.in_waiting > 0:
                rx_buffer.extend(worker.ser.read(worker.ser.in_waiting))
                worker._process_rx_buffer(rx_buffer)
                for log in logs:
                    if "[CFG-ACK]" in log:
                        ack_received = log
                        break
                if ack_received:
                    break
            time.sleep(0.01)

        if not ack_received:
            raise RuntimeError(f"MCU did not reply with [CFG-ACK] after flashing! Captured logs: {logs}")

        print(f"[STAGE 2C] SUCCESS: Post-Flash Smoke Test Passed! MCU replied -> {ack_received}")
    finally:
        if worker.ser and worker.ser.is_open:
            worker.ser.close()


def main():
    parser = argparse.ArgumentParser(description="Stage 2 CI/CT: Headless Firmware Build, Flash & Smoke Test")
    parser.add_argument(
        "--force-flash",
        action="store_true",
        help="Always flash the NXP board even if git diff shows no C firmware changes.",
    )
    parser.add_argument(
        "--skip-flash",
        action="store_true",
        help="Compile firmware only without flashing hardware.",
    )
    args = parser.parse_args()

    env = setup_toolchain_env()
    fw_changed = has_firmware_changed()

    build_firmware(env)

    if args.skip_flash:
        print("\n[STAGE 2] --skip-flash specified. Skipping LinkServer hardware programming.")
        return

    if args.force_flash or fw_changed:
        device_id = detect_linkserver_device()
        flash_firmware(device_id)
    else:
        print("\n[STAGE 2B] No C firmware changes detected in Git. Skipping flash (use --force-flash to override).")

    verify_mcu_boot_and_handshake(COM_PORT, BAUD_RATE)


if __name__ == "__main__":
    main()