#!/bin/bash
set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
ROOT_DIR="$(dirname "$SCRIPT_DIR")"
ELF="$ROOT_DIR/build/freertos-sensor-monitor.elf"

if ! command -v qemu-system-arm &>/dev/null; then
    echo "[ERROR] qemu-system-arm not found."
    echo "  macOS  : brew install qemu"
    echo "  Ubuntu : sudo apt install qemu-system-arm"
    exit 1
fi

if [ ! -f "$ELF" ]; then
    echo "[ERROR] ELF not found. Run scripts/build.sh first."
    exit 1
fi

echo "Starting QEMU (lm3s6965evb)..."
echo "UART output will appear below. Press Ctrl+A then X to quit."
echo "-----------------------------------------------------------"

qemu-system-arm \
    -machine lm3s6965evb \
    -cpu cortex-m3 \
    -kernel "$ELF" \
    -serial mon:stdio \
    -nographic \
    -semihosting-config enable=on,target=native
