#include "timer.h"
#include "io.h"
#include "process.h"

static uint32_t tick = 0;

void timer_init(uint32_t frequency) {
    uint32_t divisor = 1193180 / frequency;
    outb(0x43, 0x36);
    outb(0x40, divisor & 0xFF);
    outb(0x40, (divisor >> 8) & 0xFF);
}

void timer_handler() {
    tick++;
    if (tick % 10 == 0) {
        schedule();
    }
}

uint32_t get_ticks() {
    return tick;
}

void beep(uint32_t frequency, uint32_t duration_ms) {
    uint32_t div = 1193180 / frequency;
    outb(0x43, 0xB6);
    outb(0x42, div & 0xFF);
    outb(0x42, (div >> 8) & 0xFF);
    uint8_t tmp = inb(0x61);
    outb(0x61, tmp | 3);
    for (uint32_t i = 0; i < duration_ms * 1000; i++) __asm__("nop");
    outb(0x61, tmp & 0xFC);
}