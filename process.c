#include "process.h"
#include "kheap.h"
#include "string.h"

static process_t processes[MAX_PROCESSES];
static process_t *current = NULL;
static process_t *ready_queue = NULL;
static uint32_t next_pid = 1;

extern void switch_to(context_t *old, context_t *new);

void scheduler_init() {
    memset(processes, 0, sizeof(processes));
    process_t *idle = &processes[0];
    idle->pid = 0;
    idle->state = PROC_RUNNING;
    idle->priority = 0;
    idle->kernel_stack = 0;
    idle->user_stack = 0;
    idle->page_directory = 0;
    idle->next = NULL;
    current = idle;
}

int create_process(void (*entry)(), uint32_t user_stack, uint32_t kernel_stack) {
    int slot = -1;
    for (int i = 1; i < MAX_PROCESSES; i++) {
        if (processes[i].pid == 0) { slot = i; break; }
    }
    if (slot == -1) return -1;
    process_t *proc = &processes[slot];
    proc->pid = next_pid++;
    proc->state = PROC_READY;
    proc->priority = 1;
    if (!kernel_stack) {
        kernel_stack = (uint32_t)kmalloc(KERNEL_STACK_SIZE) + KERNEL_STACK_SIZE - 4;
    }
    if (!user_stack) {
        user_stack = (uint32_t)kmalloc(USER_STACK_SIZE) + USER_STACK_SIZE - 4;
    }
    proc->kernel_stack = kernel_stack;
    proc->user_stack = user_stack;
    proc->page_directory = 0;
    memset(&proc->context, 0, sizeof(context_t));
    proc->context.eip = (uint32_t)entry;
    proc->context.eflags = 0x202;
    proc->context.esp = user_stack;
    proc->next = ready_queue;
    ready_queue = proc;
    return proc->pid;
}

void exit_process(int status) {
    (void)status;
    if (!current || current->pid == 0) return;
    current->state = PROC_ZOMBIE;
    kfree((void*)(current->kernel_stack - KERNEL_STACK_SIZE + 4));
    kfree((void*)(current->user_stack - USER_STACK_SIZE + 4));
    schedule();
}

void schedule() {
    if (!current) return;
    if (current->state == PROC_RUNNING) {
        current->state = PROC_READY;
        if (ready_queue) {
            process_t *last = ready_queue;
            while (last->next) last = last->next;
            last->next = current;
        } else {
            ready_queue = current;
        }
    }
    process_t *next = ready_queue;
    if (next) {
        ready_queue = next->next;
        next->next = NULL;
        process_t *old = current;
        current = next;
        current->state = PROC_RUNNING;
        switch_to(&old->context, &current->context);
    } else {
        current = &processes[0];
        current->state = PROC_RUNNING;
    }
}

process_t* current_process() {
    return current;
}