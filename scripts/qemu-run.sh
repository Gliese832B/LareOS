#!/bin/bash

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
PROJECT_DIR="$(dirname "$SCRIPT_DIR")"
KERNEL="$PROJECT_DIR/build/kernel8.img"
QEMU_BIN="qemu-system-aarch64"
MODE="${1:-serial}"

if ! command -v "$QEMU_BIN" > /dev/null 2>&1; then
    echo "QEMU not found. Install it:"
    echo "  Ubuntu/Debian: sudo apt install qemu-system-arm"
    echo "  macOS:         brew install qemu"
    echo "  Windows:       choco install qemu"
    exit 1
fi

if [ ! -f "$KERNEL" ]; then
    echo "Kernel not built. Run 'make' first."
    exit 1
fi

case "$MODE" in
    serial)
        "$QEMU_BIN" \
            -M raspi3b \
            -kernel "$KERNEL" \
            -serial stdio \
            -display none \
            -no-reboot \
            -monitor none
        ;;
    gui)
        "$QEMU_BIN" \
            -M raspi3b \
            -kernel "$KERNEL" \
            -serial stdio \
            -display sdl \
            -no-reboot
        ;;
    debug)
        "$QEMU_BIN" \
            -M raspi3b \
            -kernel "$KERNEL" \
            -serial stdio \
            -display none \
            -no-reboot \
            -S -gdb tcp::1234
        ;;
    *)
        echo "Usage: $0 [serial|gui|debug]"
        exit 1
        ;;
esac
