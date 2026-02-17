#include "multiboot.h"
#include "graphics.h"
#include "fs.h"
#include "window.h"
#include "mouse.h"
#include "timer.h"
#include "process.h"
#include "syscall.h"
#include "idt.h"
#include "irq.h"
#include "isr.h"
#include "kprintf.h"

extern void irq0_handler();
extern void irq1_handler();
extern void irq12_handler();
extern void syscall_isr();

void user_program1() {
    int win = sys_create_window("User Window", 300, 200, 400, 300);
    if (win > 0) {
        sys_draw_rect(win, 0, 0, 400, 300, 0xEEEEEE);
        sys_draw_rect(win, 50, 50, 100, 100, 0xFF0000);
        sys_draw_rect(win, 150, 150, 150, 20, 0x0000FF);
    }
    while (1) {
        __asm__ volatile("hlt");
    }
}

void show_splash() {
    fill_rect(0, 0, fb_width, fb_height, 0x000000);
    draw_string("Загрузка ОС \"Российская\"", fb_width/2 - 150, fb_height/2 - 20, COLOR_WHITE, COLOR_BLACK);
    draw_string("Пожалуйста, подождите...", fb_width/2 - 120, fb_height/2 + 10, COLOR_WHITE, COLOR_BLACK);
    fill_rect(fb_width/2 - 100, fb_height/2 + 40, 200, 20, 0x404040);
    for (int i = 0; i < 30000000; i++) __asm__("nop");
}

void kernel_main(uint32_t magic, multiboot_info_t *mb_info) {
    idt_init();
    isr_init();
    irq_init();

    set_idt_gate(32, (uint32_t)irq0_handler, 0x08, 0x8E);
    set_idt_gate(33, (uint32_t)irq1_handler, 0x08, 0x8E);
    set_idt_gate(44, (uint32_t)irq12_handler, 0x08, 0x8E);
    set_idt_gate(0x80, (uint32_t)syscall_isr, 0x08, 0xEE);

    if (magic == 0x2BADB002 && (mb_info->flags & MULTIBOOT_FLAG_FB)) {
        init_graphics(mb_info);
        show_splash();

        kheap_init(0x300000, 0x100000);
        fs_init(0);
        wm_init(fb_width, fb_height, framebuffer);

        fill_rect(0, fb_height - 40, fb_width, 40, 0x404040);
        fill_rect(10, fb_height - 35, 80, 30, 0x008000);
        draw_string("Пуск", 20, fb_height - 25, COLOR_WHITE, 0x008000);
    } else {
        kheap_init(0x100000, 0x100000);
        char *video = (char*)0xB8000;
        const char *msg = "No framebuffer, text mode.";
        for (int i = 0; msg[i]; i++) {
            video[i*2] = msg[i];
            video[i*2+1] = 0x07;
        }
    }

    mouse_init();
    timer_init(100);
    scheduler_init();
    create_process(user_program1, 0, 0);

    __asm__ volatile("sti");

    while (1) {
        wm_composite();
        __asm__ volatile("hlt");
    }
}