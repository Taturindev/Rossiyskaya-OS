#ifndef IDT_H
#define IDT_H

#include "stdint.h"

void idt_init();
void set_idt_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags);
extern void idt_load();

#endif