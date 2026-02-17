global switch_to

section .text
switch_to:
    mov eax, [esp + 4]      ; old
    mov [eax], edi
    mov [eax + 4], esi
    mov [eax + 8], ebp
    mov [eax + 12], esp
    mov [eax + 16], ebx
    mov [eax + 20], edx
    mov [eax + 24], ecx
    mov [eax + 28], eax     ; сохраняем eax (будет перезаписан)
    pushf
    pop dword [eax + 36]    ; eflags
    mov ecx, [esp]          ; адрес возврата
    mov [eax + 32], ecx     ; eip

    mov eax, [esp + 8]      ; new
    mov edi, [eax]
    mov esi, [eax + 4]
    mov ebp, [eax + 8]
    mov esp, [eax + 12]
    mov ebx, [eax + 16]
    mov edx, [eax + 20]
    mov ecx, [eax + 24]
    push dword [eax + 36]   ; eflags
    popf
    push dword [eax + 32]   ; eip
    mov eax, [eax + 28]     ; восстановить eax
    ret