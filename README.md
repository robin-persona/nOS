# nOS

A 32-bit graphical operating system built from scratch, mimicking the aesthetic of Windows 7.

## Roadmap

- Phase 1: Foundation (Boot & Graphics) ✅
- **Phase 2: Interrupts & Input (GDT, IDT, PS/2 keyboard/mouse)** ✅ Current
- Phase 3: Memory Management (PMM, heap)
- Phase 4: GUI Engine (compositor, taskbar, cursor)
- Phase 5: Filesystem (initrd or FAT16)
- Phase 6: Multitasking & User Space (syscalls, ELF loading, scheduler)
- Phase 7: Userland Apps (login, notepad, snake, settings)

## Phase 1: Boot & Graphics

Boots via Multiboot into a 1024x768x32 VESA framebuffer mode and paints the screen Windows 7 Aero Blue (`#00A2ED`).

## Phase 2: Interrupts & Input

Adds:
- **GDT** (`gdt.c`, `gdt_flush.s`) — flat memory model, kernel code/data segments
- **IDT** (`idt.c`, `idt_flush.s`) — 256-entry interrupt table
- **ISRs/IRQs** (`isr.c`, `isr.s`) — CPU exception handlers (0-31) and hardware IRQ dispatch (32-47), with the 8259 PIC remapped in `pic.c` to avoid vector conflicts
- **PS/2 keyboard driver** (`keyboard.c`) — IRQ1, scancode set 1 -> ASCII (with shift handling), circular input buffer, live serial echo
- **PS/2 mouse driver** (`mouse.c`) — IRQ12, standard 3-byte packet parsing, tracks x/y/buttons clamped to the 1024x768 screen, live serial output
- **Serial debug output** (`serial.c`) — COM1 driver used for all kernel logging

### Files

```
boot.s              Multiboot bootloader
kernel.c            Kernel entry, framebuffer, init sequence
gdt.c / gdt_flush.s GDT setup
idt.c / idt_flush.s IDT setup
isr.c / isr.s        Exception + IRQ handling
pic.c                8259 PIC remap
keyboard.c           PS/2 keyboard driver
mouse.c              PS/2 mouse driver
serial.c             COM1 debug output
include/             Headers for all of the above
linker.ld            Links the kernel at 1MB
Makefile             Build system (nasm + gcc -m32 + ld + grub-mkrescue)
```

### Build & Run

```bash
# Ubuntu/Debian dependencies
sudo apt-get install build-essential gcc-multilib nasm xorriso grub-pc-bin qemu-system-x86

make
make run           # graphical window, blue screen
make run-serial     # also prints keyboard/mouse debug output to your terminal
```

Type on the keyboard or move the mouse (grab it with the QEMU window focused) and you'll see live scancode-to-ASCII and mouse coordinate output on the serial console.

## CI

Every push to `main` triggers a GitHub Actions build that compiles the kernel, boots it headlessly in QEMU as a smoke test, and uploads `nOS.bin`, `nOS.iso`, and the serial debug log as workflow artifacts. See the Actions tab for build status and downloadable ISOs.

## License

TBD
