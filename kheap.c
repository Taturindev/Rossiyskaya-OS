#include "kheap.h"

#define HEAP_MAGIC 0xDEADBEEF

typedef struct heap_block {
    uint32_t magic;
    uint32_t size;
    uint8_t free;
    struct heap_block *next;
} heap_block_t;

static heap_block_t *heap_start = 0;

void kheap_init(uint32_t start, uint32_t size) {
    heap_start = (heap_block_t*)start;
    heap_start->magic = HEAP_MAGIC;
    heap_start->size = size - sizeof(heap_block_t);
    heap_start->free = 1;
    heap_start->next = 0;
}

void *kmalloc(uint32_t size) {
    heap_block_t *block = heap_start;
    while (block) {
        if (block->magic != HEAP_MAGIC) return 0;
        if (block->free && block->size >= size) {
            if (block->size > size + sizeof(heap_block_t) + 4) {
                heap_block_t *new_block = (heap_block_t*)((uint32_t)block + sizeof(heap_block_t) + size);
                new_block->magic = HEAP_MAGIC;
                new_block->size = block->size - size - sizeof(heap_block_t);
                new_block->free = 1;
                new_block->next = block->next;
                block->next = new_block;
                block->size = size;
            }
            block->free = 0;
            return (void*)((uint32_t)block + sizeof(heap_block_t));
        }
        block = block->next;
    }
    return 0;
}

void kfree(void *ptr) {
    if (!ptr) return;
    heap_block_t *block = (heap_block_t*)((uint32_t)ptr - sizeof(heap_block_t));
    if (block->magic != HEAP_MAGIC) return;
    block->free = 1;
    heap_block_t *curr = heap_start;
    while (curr && curr->next) {
        if (curr->free && curr->next->free) {
            curr->size += sizeof(heap_block_t) + curr->next->size;
            curr->next = curr->next->next;
        } else {
            curr = curr->next;
        }
    }
}