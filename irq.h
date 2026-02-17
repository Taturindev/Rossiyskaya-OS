#ifndef IRQ_H
#define IRQ_H

#include "stdint.h"

void irq_install_handler(int irq, void (*handler)());
void irq_uninstall_handler(int irq);
void irq_init();

#endif