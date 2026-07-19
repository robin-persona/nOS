# Makefile for nOS - Phase 2

# Compiler and tools
AS = nasm
CC = gcc
LD = ld

# Flags
ASFLAGS = -f elf32
CFLAGS = -m32 -ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-stack-protector -nostdlib -nostartfiles -Iinclude
LDFLAGS = -m elf_i386 -T linker.ld

# Source files (boot.s must link first so the Multiboot header lands early)
ASM_SRC = boot.s gdt_flush.s idt_flush.s isr.s
C_SRC = kernel.c gdt.c idt.c isr.c pic.c keyboard.c mouse.c serial.c

# Object files
ASM_OBJ = $(ASM_SRC:.s=.o)
C_OBJ = $(C_SRC:.c=.o)
OBJS = $(ASM_OBJ) $(C_OBJ)

# Output
KERNEL = nOS.bin
ISO = nOS.iso
ISODIR = isodir

# Default target
all: $(ISO)

# Assemble .s files
%.o: %.s
	$(AS) $(ASFLAGS) $< -o $@

# Compile .c files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Link kernel
$(KERNEL): $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $(OBJS)

# Create bootable ISO
$(ISO): $(KERNEL)
	mkdir -p $(ISODIR)/boot/grub
	cp $(KERNEL) $(ISODIR)/boot/$(KERNEL)
	echo 'menuentry "nOS" {' > $(ISODIR)/boot/grub/grub.cfg
	echo '    multiboot /boot/$(KERNEL)' >> $(ISODIR)/boot/grub/grub.cfg
	echo '    boot' >> $(ISODIR)/boot/grub/grub.cfg
	echo '}' >> $(ISODIR)/boot/grub/grub.cfg
	grub-mkrescue -o $(ISO) $(ISODIR)

# Run in QEMU
run: $(ISO)
	qemu-system-i386 -cdrom $(ISO) -m 128M

# Run in QEMU with serial output visible in the terminal (see keyboard/mouse debug logs)
run-serial: $(ISO)
	qemu-system-i386 -cdrom $(ISO) -m 128M -serial stdio

# Run in QEMU with specific graphics
run-vga: $(ISO)
	qemu-system-i386 -cdrom $(ISO) -m 128M -vga std

# Clean build artifacts
clean:
	rm -f $(OBJS) $(KERNEL)
	rm -rf $(ISODIR)

# Clean everything including ISO
distclean: clean
	rm -f $(ISO)

# Debug info
debug:
	@echo "ASM Objects: $(ASM_OBJ)"
	@echo "C Objects: $(C_OBJ)"
	@echo "All Objects: $(OBJS)"
	@echo "Kernel: $(KERNEL)"
	@echo "ISO: $(ISO)"

.PHONY: all run run-serial run-vga clean distclean debug
