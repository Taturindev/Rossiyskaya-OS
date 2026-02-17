#include "irq.h"
#include "io.h"
#include "mouse.h"
#include "timer.h"

void *irq_routines[16] = { 0 };

void irq_install_handler(int irq, void (*handler)()) {
    irq_routines[irq] = handler;
}

void irq_uninstall_handler(int irq) {
    irq_routines[irq] = 0;
}

void irq_handler(int irq) {
    if (irq_routines[irq]) {
        void (*handler)() = irq_routines[irq];
        handler();
    }
    if (irq >= 40) {
        outb(0xA0, 0x20);
    }
    outb(0x20, 0x20);
}

void irq_init() {
    outb(0x20, 0x11);
    outb(0xA0, 0x11);
    outb(0x21, 0x20);
    outb(0xA1, 0x28);
    outb(0x21, 0x04);
    outb(0xA1, 0x02);
    outb(0x21, 0x01);
    outb(0xA1, 0x01);
    outb(0x21, 0x0);
    outb(0xA1, 0x0);
}