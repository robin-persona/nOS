# nOS

A 32-bit graphical operating system built from scratch, mimicking the aesthetic of Windows 7.

## Roadmap

- **Phase 1: Foundation (Boot & Graphics)** ✅ Current
- Phase 2: Interrupts & Input (GDT, IDT, PS/2 keyboard/mouse)
- Phase 3: Memory Management (PMM, heap)
- Phase 4: GUI Engine (compositor, taskbar, cursor)
- Phase 5: Filesystem (initrd or FAT16)
- Phase 6: Multitasking & User Space (syscalls, ELF loading, scheduler)
- Phase 7: Userland Apps (login, notepad, snake, settings)

## Phase 1: Boot & Graphics

Boots via Multiboot into a 1024x768x32 VESA framebuffer mode and paints the screen Windows 7 Aero Blue (`#00A2ED`).

### Files

- `boot.s` - Multiboot-compliant assembly bootloader, sets up the stack and jumps to `kernel_main`
- `kernel.c` - Initializes the framebuffer from Multiboot info and paints the screen
- `linker.ld` - Links the kernel at 1MB
- `Makefile` - Build system (nasm + gcc -m32 + ld + grub-mkrescue)

### Build & Run

```bash
# Ubuntu/Debian dependencies
sudo apt-get install build-essential nasm xorriso grub-pc-bin qemu-system-x86

make
make run
```

Expected output: a solid Windows 7 blue (`#00A2ED`) screen at 1024x768 in QEMU.

## License

TBD
