CROSS = aarch64-none-elf-
CC = $(CROSS)gcc
AS = $(CROSS)as
LD = $(CROSS)ld
OBJCOPY = $(CROSS)objcopy

CFLAGS = -Wall -Wextra -ffreestanding -nostdlib -nostartfiles -mgeneral-regs-only -Iinclude -O2
LDFLAGS = -nostdlib -T boot/linker.ld

BUILD = build
TARGET = $(BUILD)/kernel8.img

BOOT_SRC = boot/boot.S
KERNEL_SRC = kernel/kernel.c kernel/mm.c kernel/power.c kernel/shell.c
DRIVER_SRC = drivers/gpio.c drivers/uart.c drivers/mailbox.c drivers/timer.c drivers/irq.c drivers/fb.c
LIB_SRC = lib/string.c

ASM_OBJ = $(BUILD)/boot.o
C_OBJ = $(patsubst %.c,$(BUILD)/%.o,$(notdir $(KERNEL_SRC) $(DRIVER_SRC) $(LIB_SRC)))
OBJECTS = $(ASM_OBJ) $(C_OBJ)

.PHONY: all clean qemu dirs

all: dirs $(TARGET)

dirs:
	@mkdir -p $(BUILD)

$(BUILD)/boot.o: boot/boot.S
	$(AS) -o $@ $<

$(BUILD)/kernel.o: kernel/kernel.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD)/mm.o: kernel/mm.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD)/power.o: kernel/power.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD)/shell.o: kernel/shell.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD)/gpio.o: drivers/gpio.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD)/uart.o: drivers/uart.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD)/mailbox.o: drivers/mailbox.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD)/timer.o: drivers/timer.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD)/irq.o: drivers/irq.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD)/fb.o: drivers/fb.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD)/string.o: lib/string.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD)/kernel8.elf: $(OBJECTS)
	$(LD) $(LDFLAGS) -o $@ $^

$(TARGET): $(BUILD)/kernel8.elf
	$(OBJCOPY) -O binary $< $@

clean:
	rm -rf $(BUILD)

qemu: all
	qemu-system-aarch64 \
		-M raspi3b \
		-kernel $(TARGET) \
		-serial stdio \
		-display none \
		-no-reboot

qemu-gui: all
	qemu-system-aarch64 \
		-M raspi3b \
		-kernel $(TARGET) \
		-serial stdio \
		-display sdl

rpi4: CFLAGS += -DRPI4
rpi4: all
