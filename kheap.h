#ifndef KHEAP_H
#define KHEAP_H

#include "stdint.h"

void kheap_init(uint32_t start, uint32_t size);
void *kmalloc(uint32_t size);
void kfree(void *ptr);

#endif