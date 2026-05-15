#!/bin/bash
set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
ROOT_DIR="$(dirname "$SCRIPT_DIR")"
BUILD_DIR="$ROOT_DIR/build"

# Check dependencies
check_dep() {
    if ! command -v "$1" &>/dev/null; then
        echo "[ERROR] $1 not found. $2"
        exit 1
    fi
}

check_dep arm-none-eabi-gcc "Install: brew install --cask gcc-arm-embedded  (macOS) | sudo apt install gcc-arm-none-eabi  (Ubuntu)"
check_dep cmake              "Install: brew install cmake  |  sudo apt install cmake"

# Init submodule if missing
if [ ! -f "$ROOT_DIR/FreeRTOS-Kernel/tasks.c" ]; then
    echo "[INFO] Initializing FreeRTOS-Kernel submodule..."
    git -C "$ROOT_DIR" submodule update --init --recursive
fi

mkdir -p "$BUILD_DIR"
cmake -S "$ROOT_DIR" -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE=Debug
cmake --build "$BUILD_DIR" -j"$(nproc 2>/dev/null || sysctl -n hw.ncpu)"

echo ""
echo "Build successful!"
echo "  ELF : $BUILD_DIR/freertos-sensor-monitor.elf"
echo "  BIN : $BUILD_DIR/freertos-sensor-monitor.bin"
echo "  HEX : $BUILD_DIR/freertos-sensor-monitor.hex"
