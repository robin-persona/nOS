// kernel.c - nOS Kernel Entry Point (Phase 3 + 4)
// Adds physical memory management, a kernel heap, and a live GUI compositor
// (double-buffered desktop with taskbar, Start button, a demo window, and
// a PS/2-driven cursor).

#include <stdint.h>
#include <stddef.h>
#include "multiboot.h"
#include "gdt.h"
#include "idt.h"
#include "isr.h"
#include "keyboard.h"
#include "mouse.h"
#include "serial.h"
#include "pmm.h"
#include "heap.h"
#include "gfx.h"
#include "compositor.h"

#define MULTIBOOT_MAGIC 0x2BADB002
#define WIN7_BLUE 0x00A2ED

static uint32_t *framebuffer = NULL;
static uint32_t fb_width = 0;
static uint32_t fb_height = 0;
static uint32_t fb_pitch = 0;
static uint8_t fb_bpp = 0;

void *memset(void *dst, int c, size_t n) {
    uint8_t *d = (uint8_t *)dst;
    while (n-- > 0) {
        *d++ = (uint8_t)c;
    }
    return dst;
}

int framebuffer_init(struct multiboot_info *mbi) {
    if (!(mbi->flags & (1 << 12))) {
        return -1;
    }

    framebuffer = (uint32_t *)(uintptr_t)mbi->framebuffer_addr;
    fb_width = mbi->framebuffer_width;
    fb_height = mbi->framebuffer_height;
    fb_pitch = mbi->framebuffer_pitch;
    fb_bpp = mbi->framebuffer_bpp;

    if (framebuffer == NULL || fb_width == 0 || fb_height == 0) {
        return -1;
    }

    return 0;
}

static void clear_screen(uint32_t color) {
    if (framebuffer == NULL) return;
    uint32_t pixel_count = fb_width * fb_height;
    for (uint32_t i = 0; i < pixel_count; i++) {
        framebuffer[i] = color;
    }
}

void kernel_main(uint32_t magic, struct multiboot_info *mbi) {
    if (magic != MULTIBOOT_MAGIC) {
        while (1) { __asm__ volatile ("hlt"); }
    }

    if (framebuffer_init(mbi) != 0) {
        while (1) { __asm__ volatile ("hlt"); }
    }

    clear_screen(WIN7_BLUE); // immediate visual feedback before the GUI engine spins up

    serial_init();
    serial_write("\n=== nOS Phase 3 + 4: Memory & GUI Engine ===\n");

    gdt_install();
    serial_write("[OK] GDT installed\n");

    idt_install();
    serial_write("[OK] IDT installed\n");

    isr_install();
    serial_write("[OK] ISRs/IRQs installed, PIC remapped\n");

    keyboard_init();
    serial_write("[OK] Keyboard driver initialized (IRQ1)\n");

    mouse_init();
    serial_write("[OK] Mouse driver initialized (IRQ12)\n");

    __asm__ volatile ("sti");
    serial_write("[OK] Interrupts enabled\n");

    pmm_init(mbi);
    serial_write("[OK] Physical memory manager initialized\n");

    heap_init();
    serial_write("[OK] Kernel heap initialized\n");

    gfx_init(framebuffer, fb_width, fb_height);
    serial_write("[OK] GFX double buffer allocated\n");

    serial_write("[OK] Entering compositor render loop\n\n");

    while (1) {
        compositor_render();
        gfx_swap();
    }
}
