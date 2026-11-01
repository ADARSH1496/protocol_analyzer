import argparse
import os
import subprocess
import sys
import time

# Force UTF-8 on Windows background pipes (GitHub Actions / Jenkins cmd.exe runners)
if hasattr(sys.stdout, "reconfigure"):
    sys.stdout.reconfigure(encoding="utf-8", errors="replace")
if hasattr(sys.stderr, "reconfigure"):
    sys.stderr.reconfigure(encoding="utf-8", errors="replace")

SOFTWARE_DIR = os.path.dirname(os.path.abspath(__file__))
PROJECT_ROOT = os.path.abspath(os.path.join(SOFTWARE_DIR, ".."))

UNIT_TEST_SCRIPT = os.path.join(SOFTWARE_DIR, "tests", "test_unit_software.py")
HIL_TEST_SCRIPT = os.path.join(SOFTWARE_DIR, "tests", "test_hil_hardware.py")
BUILD_FLASH_SCRIPT = os.path.join(SOFTWARE_DIR, "scripts", "build_and_flash.py")
PACKAGE_SCRIPT = os.path.join(SOFTWARE_DIR, "scripts", "package_release.py")
HTML_REPORT_PATH = os.path.join(SOFTWARE_DIR, "reports", "validation_report.html")


def run_stage(stage_name: str, cmd: list[str], cwd: str) -> float:
    print("\n" + "=" * 80)
    print(f"[START] {stage_name}")
    print(f"        Command: {' '.join(cmd)}")
    print("=" * 80)

    env = os.environ.copy()
    env["PYTHONUTF8"] = "1"
    env["PYTHONIOENCODING"] = "utf-8"

    start_t = time.time()
    result = subprocess.run(cmd, cwd=cwd, env=env)
    elapsed = time.time() - start_t

    if result.returncode != 0:
        print("\n" + "!" * 80)
        print(f"[FAIL] PIPELINE ABORTED: {stage_name} FAILED (Exit Code {result.returncode}) after {elapsed:.1f}s")
        print("       Quality Gate blocked downstream stages from executing.")
        print("!" * 80)
        sys.exit(result.returncode)

    print(f"\n[PASS] {stage_name} PASSED ({elapsed:.1f}s)")
    return elapsed


def main():
    parser = argparse.ArgumentParser(description="Master 4-Stage CI/CT/CD Pipeline Runner")
    parser.add_argument(
        "--force-flash",
        action="store_true",
        help="Force LinkServer hardware flashing even if C firmware files did not change.",
    )
    parser.add_argument(
        "--skip-cd",
        action="store_true",
        help="Run CI + CT stages only and skip Stage 4 PyInstaller .exe packaging.",
    )
    args = parser.parse_args()

    pipeline_start = time.time()
    timings = {}

    # -------------------------------------------------------------------------
    # STAGE 1: CI - OFFLINE SOFTWARE & HEADLESS GUI UNIT TESTS (10 TESTS)
    # -------------------------------------------------------------------------
    timings["Stage 1: Software & GUI Unit Tests (CI)"] = run_stage(
        "STAGE 1: CI - Offline Software & Headless GUI Unit Tests",
        [sys.executable, "-m", "pytest", UNIT_TEST_SCRIPT, "-v"],
        cwd=SOFTWARE_DIR,
    )

    # -------------------------------------------------------------------------
    # STAGE 2: CI/CT - EMBEDDED C FIRMWARE BUILD, FLASH & UART SMOKE TEST
    # -------------------------------------------------------------------------
    stage2_cmd = [sys.executable, BUILD_FLASH_SCRIPT]
    if args.force_flash:
        stage2_cmd.append("--force-flash")

    timings["Stage 2: Firmware Build, Flash & Smoke Check"] = run_stage(
        "STAGE 2: CI/CT - Headless C Firmware Build, Flash & Smoke Test",
        stage2_cmd,
        cwd=SOFTWARE_DIR,
    )

    # -------------------------------------------------------------------------
    # STAGE 3: CT - LIVE HARDWARE-IN-THE-LOOP (HIL) PYTEST & HTML CERTIFICATE
    # -------------------------------------------------------------------------
    timings["Stage 3: Live HIL PyTest Suite & HTML Report (CT)"] = run_stage(
        "STAGE 3: CT - Live Hardware-in-the-Loop (HIL) Verification Suite",
        [
            sys.executable,
            "-m",
            "pytest",
            UNIT_TEST_SCRIPT,
            HIL_TEST_SCRIPT,
            "-v",
            f"--html={HTML_REPORT_PATH}",
            "--self-contained-html",
        ],
        cwd=SOFTWARE_DIR,
    )

    # -------------------------------------------------------------------------
    # STAGE 4: CD - STANDALONE .EXE PACKAGING & RELEASE BUNDLE ASSEMBLY
    # -------------------------------------------------------------------------
    if not args.skip_cd:
        timings["Stage 4: PyInstaller .exe & Release Bundle (CD)"] = run_stage(
            "STAGE 4: CD - Standalone .exe Packaging & Release Artifact Bundle",
            [sys.executable, PACKAGE_SCRIPT],
            cwd=SOFTWARE_DIR,
        )

    total_time = time.time() - pipeline_start
    print("\n" + "=" * 80)
    print("[SUCCESS] 4-STAGE CI / CT / CD PIPELINE COMPLETED SUCCESSFULLY!")
    print("=" * 80)
    for stage, duration in timings.items():
        print(f"  [OK] {stage:<50} : {duration:>6.1f}s")
    print("-" * 80)
    print(f"  TOTAL PIPELINE DURATION                             : {total_time:>6.1f}s")
    print(f"  RELEASE BUNDLE LOCATION                             : {os.path.join(PROJECT_ROOT, 'release_package')}")
    print("=" * 80)


if __name__ == "__main__":
    main()