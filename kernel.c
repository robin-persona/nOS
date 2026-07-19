// kernel.c - nOS Kernel Entry Point (Phase 2)
// Adds GDT, IDT, interrupt handling, PS/2 keyboard + mouse drivers.

#include <stdint.h>
#include <stddef.h>
#include "gdt.h"
#include "idt.h"
#include "isr.h"
#include "keyboard.h"
#include "mouse.h"
#include "serial.h"

// Multiboot magic number
#define MULTIBOOT_MAGIC 0x2BADB002

// Windows 7 Aero Blue color (RGB: 0, 162, 237 -> 0x00A2ED)
#define WIN7_BLUE 0x00A2ED

// Multiboot info structure (simplified)
struct multiboot_info {
    uint32_t flags;
    uint32_t mem_lower;
    uint32_t mem_upper;
    uint32_t boot_device;
    uint32_t cmdline;
    uint32_t mods_count;
    uint32_t mods_addr;
    uint32_t syms[4];
    uint32_t mmap_length;
    uint32_t mmap_addr;
    uint32_t drives_length;
    uint32_t drives_addr;
    uint32_t config_table;
    uint32_t boot_loader_name;
    uint32_t apm_table;
    uint32_t vbe_control_info;
    uint32_t vbe_mode_info;
    uint16_t vbe_mode;
    uint16_t vbe_interface_seg;
    uint16_t vbe_interface_off;
    uint16_t vbe_interface_len;
    uint64_t framebuffer_addr;
    uint32_t framebuffer_pitch;
    uint32_t framebuffer_width;
    uint32_t framebuffer_height;
    uint8_t  framebuffer_bpp;
    uint8_t  framebuffer_type;
    uint8_t  color_info[6];
} __attribute__((packed));

// Global framebuffer variables
static uint32_t *framebuffer = NULL;
static uint32_t fb_width = 0;
static uint32_t fb_height = 0;
static uint32_t fb_pitch = 0;
static uint8_t fb_bpp = 0;

// Simple memset implementation
void *memset(void *dst, int c, size_t n) {
    uint8_t *d = (uint8_t *)dst;
    while (n-- > 0) {
        *d++ = (uint8_t)c;
    }
    return dst;
}

// Initialize framebuffer from Multiboot info
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

void clear_screen(uint32_t color) {
    if (framebuffer == NULL) return;

    uint32_t pixel_count = fb_width * fb_height;
    for (uint32_t i = 0; i < pixel_count; i++) {
        framebuffer[i] = color;
    }
}

void put_pixel(uint32_t x, uint32_t y, uint32_t color) {
    if (x >= fb_width || y >= fb_height) return;
    framebuffer[y * fb_width + x] = color;
}

// Kernel main function
void kernel_main(uint32_t magic, struct multiboot_info *mbi) {
    if (magic != MULTIBOOT_MAGIC) {
        while (1) { __asm__ volatile ("hlt"); }
    }

    if (framebuffer_init(mbi) != 0) {
        while (1) { __asm__ volatile ("hlt"); }
    }

    clear_screen(WIN7_BLUE);

    // --- Phase 2: Interrupts & Input ---
    serial_init();
    serial_write("\n=== nOS Phase 2: Interrupts & Input ===\n");

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
    serial_write("[OK] Interrupts enabled - type on keyboard or move mouse!\n\n");

    while (1) {
        __asm__ volatile ("hlt");
    }
}
