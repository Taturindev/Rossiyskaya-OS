.section .multiboot
.align 4
.long 0x1BADB002
.long 0x10003
.long -(0x1BADB002 + 0x10003)
.long 0
.long 1024
.long 768
.long 32

.section .text
.global _start
_start:
    mov $stack_top, %esp
    push %ebx
    push %eax
    call kernel_main
    cli
.halt:
    hlt
    jmp .halt

.section .bss
.space 16384
stack_top: