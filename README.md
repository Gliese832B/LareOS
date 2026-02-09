# LareOS

LareOS is an AArch64 Bare-Metal Operating System for the Raspberry Pi, written in C and Assembly. It runs directly on the hardware.

---

## Features

- **AArch64 Architecture:** 64-bit ARM assembly and C implementation.
- **UART Serial Console:** Serial terminal interface.
- **Framebuffer Graphics:** 800x600x32-bit resolution with a graphics library.
- **Memory Management:** Page and Heap allocator.
- **Interrupt Handling (IRQ):** Hardware interrupts and system timer.
- **Interactive Shell:** Built-in shell with 16+ commands.
- **Power Management:** CPU temperature monitoring, frequency scaling, and performance profiles (max, balanced, powersave).
- **Benchmark Suite:** Hardware performance tests.

---

## Build and Installation

### Prerequisites

- Cross-Compiler: `aarch64-none-elf-gcc`
- QEMU: For Raspberry Pi 3 emulation.
- Make: For build automation.

### Compilation

To compile, run:

```bash
make clean
make
```

The process generates `build/kernel8.img`.

---

### Using QEMU

Serial Mode:
```bash
make qemu
```

Graphic Mode:
```bash
make qemu-gui
```

## Shell Commands

| Command | Description |
| :--- | :--- |
| `help` | Lists commands |
| `info` | Displays system and hardware information |
| `status` | Reports CPU and RAM usage |
| `temp` | Reads CPU temperature |
| `profile` | Changes performance profiles |
| `benchmark` | Runs performance tests |
| `clear` | Clears the screen |

---

## Project Structure

```text
LareOS/
├── boot/       # Bootloader and Linker scripts
├── drivers/    # GPIO, UART, Timer, FB, Mailbox drivers
├── include/    # Header files
├── kernel/     # Core logic, Memory Management, Shell, Power
├── lib/        # C libraries
└── scripts/    # QEMU scripts
```

---

## License

MIT License.

---
> **Note:** Still not completed.
