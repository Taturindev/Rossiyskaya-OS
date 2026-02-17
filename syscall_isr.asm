global syscall_isr
extern syscall_handler

syscall_isr:
    pusha
    push ds
    push es
    push fs
    push gs
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    push edi
    push esi
    push edx
    push ecx
    push ebx
    push eax
    call syscall_handler
    add esp, 24
    mov [esp + 28], eax  ; replace saved eax with return value
    pop gs
    pop fs
    pop es
    pop ds
    popa
    iret