#include "isr.h"
#include "idt.h"

extern void isr0();

void isr_init() {
    set_idt_gate(0, (uint32_t)isr0, 0x08, 0x8E);
    // можно добавить остальные
}