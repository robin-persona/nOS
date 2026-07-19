# nOS

A 32-bit graphical operating system built from scratch, mimicking the aesthetic of Windows 7.

## Roadmap

- Phase 1: Foundation (Boot & Graphics) ✅
- Phase 2: Interrupts & Input (GDT, IDT, PS/2 keyboard/mouse) ✅
- **Phase 3: Memory Management (PMM, heap)** ✅ Current
- **Phase 4: GUI Engine (compositor, taskbar, cursor)** ✅ Current
- Phase 5: Filesystem (initrd or FAT16) — not yet started
- Phase 6: Multitasking & User Space (syscalls, ELF loading, scheduler) — not yet started
- Phase 7: Userland Apps (login, notepad, snake, settings) — not yet started

> Phases 5-7 involve disk I/O, ELF loading, and a real preemptive scheduler with
> hand-written context-switch assembly — these are being built incrementally and
> tested in isolation rather than in one shot, since a single mistake there tends
> to produce silent stack corruption or triple-fault reboots that are hard to
> debug from CI logs alone.

## Phase 1: Boot & Graphics

Boots via Multiboot into a 1024x768x32 VESA framebuffer mode.

## Phase 2: Interrupts & Input

GDT, IDT, 8259 PIC remap, PS/2 keyboard + mouse drivers, COM1 serial debug output.

## Phase 3: Memory Management

- **`pmm.c`** — bitmap-based physical frame allocator (4KB frames). Frames below 1MB and those occupied by the kernel image are reserved at boot; everything else is available via `pmm_alloc_frame()` / `pmm_free_frame()`. No paging yet, so physical addresses are used directly as pointers.
- **`heap.c`** — an 8MB kernel heap reserved from the PMM at boot, managed as an explicit first-fit free list (`kmalloc` / `kfree`, with basic adjacent-block coalescing on free).

## Phase 4: GUI Engine

- **`gfx.c`** — a heap-backed backbuffer with `gfx_fill_rect`, `gfx_put_pixel`, text drawing, and `gfx_swap()` to blit to the real framebuffer (eliminates flicker vs. drawing directly to video memory).
- **`font.c`** — a compact 5x7 bitmap font (digits, A-Z, space, `-`, `:`, `.`).
- **`compositor.c`** — renders the desktop every frame: Windows 7 blue background, a demo window with a title bar and close button, a taskbar with a Start button, and a live cursor tracking the PS/2 mouse driver from Phase 2.

The kernel's main loop now continuously calls `compositor_render()` + `gfx_swap()` instead of halting — move the mouse or watch the cursor track in real time.

### Files

```
boot.s                    Multiboot bootloader
kernel.c                  Kernel entry, init sequence, compositor render loop
gdt.c / gdt_flush.s        GDT setup
idt.c / idt_flush.s        IDT setup
isr.c / isr_stubs.s        Exception + IRQ handling
pic.c                      8259 PIC remap
keyboard.c                 PS/2 keyboard driver
mouse.c                    PS/2 mouse driver
serial.c                   COM1 debug output
pmm.c                      Physical memory manager (bitmap frame allocator)
heap.c                     Kernel heap (kmalloc/kfree)
gfx.c                      Double-buffered drawing primitives
font.c                     5x7 bitmap font
compositor.c               Desktop rendering: taskbar, Start button, window, cursor
include/                   Headers for all of the above
linker.ld                  Links the kernel at 1MB
Makefile                   Build system (nasm + gcc -m32 + ld + grub-mkrescue)
```

### Build & Run

```bash
# Ubuntu/Debian dependencies
sudo apt-get install build-essential gcc-multilib nasm xorriso grub-pc-bin qemu-system-x86

make
make run           # graphical desktop: taskbar, Start button, demo window, live cursor
make run-serial     # also prints PMM/heap/keyboard/mouse debug output to your terminal
```

## CI

Every push to `main` triggers a GitHub Actions build that compiles the kernel, boots it headlessly in QEMU as a smoke test, and uploads `nOS.bin`, `nOS.iso`, and the serial debug log as workflow artifacts.

## License

TBD
