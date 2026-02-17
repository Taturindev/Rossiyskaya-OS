#ifndef PROCESS_H
#define PROCESS_H

#include "stdint.h"

#define MAX_PROCESSES     64
#define KERNEL_STACK_SIZE 16384
#define USER_STACK_SIZE   8192

#define PROC_READY        1
#define PROC_RUNNING      2
#define PROC_WAITING      3
#define PROC_ZOMBIE       4

typedef struct {
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
    uint32_t eip, eflags;
} context_t;

typedef struct process {
    uint32_t pid;
    uint32_t state;
    uint32_t priority;
    uint32_t kernel_stack;
    uint32_t user_stack;
    uint32_t page_directory;
    context_t context;
    struct process *next;
} process_t;

void scheduler_init();
int create_process(void (*entry)(), uint32_t user_stack, uint32_t kernel_stack);
void exit_process(int status);
void schedule();
process_t* current_process();

#endif