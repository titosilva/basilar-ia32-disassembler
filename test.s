section .data
error_msg db "Error", 0dH, 0aH
error_msg_length EQU $-error_msg

section .bss
temp_str resb 100

section .text

; int _puts(int, char*)
_puts:
    enter 8, 0

    mov dword eax, [ebp+12]
    mov dword [ebp-4], eax

    mov dword eax, [ebp+8]
    mov dword [ebp-8], eax
    push ebx

    mov dword eax, 4
    mov dword ebx, 1
    mov dword ecx, [ebp-4]
    mov dword edx, [ebp-8]
    int 0x80

    pop ebx
    leave
    ret

; int _prints(char*)
_prints:
    enter 0, 0
    push ebx
    push esi

    mov ebx, [ebp+8]

__loop_prints:
    cmp byte [ebx], 0
    je __end_prints

    mov eax, 4
    mov ebx, 1
    mov ecx, ebx
    mov edx, 1
    int 0x80

    inc ebx
    jmp __loop_prints

__end_prints:
    pop esi
    pop ebx
    leave
    ret

; int _gets(int, char*)
_gets:
    enter 8, 0

    mov dword eax, [ebp+12]
    mov dword [ebp-4], eax

    mov dword eax, [ebp+8]
    mov dword [ebp-8], eax
    push ebx

    mov eax, 3
    mov ebx, 0
    mov ecx, [ebp-4]
    mov edx, [ebp-8]
    int 0x80

    pop ebx
    leave
    ret

; int _reads(char*) 
; reads a string from stdin until a newline character is found
_reads:
    enter 0, 0
    push ebx
    push esi

    mov ebx, [ebp+8]

__loop_reads:
    mov eax, 3
    mov ebx, 0
    mov ecx, ebx
    mov edx, 1
    int 0x80

    cmp byte [ebx], 0x0A
    je __end_reads

    inc ebx
    jmp __loop_reads

__end_reads:
    mov byte [ebx], 0
    pop esi
    pop ebx
    leave
    ret

; int atoi(char*)
_atoi:
    enter 0,0
    push ebx
    push esi
    push ecx
    
    mov dword ebx, [ebp+8]
    mov dword esi, 0
    mov dword eax, 0

__loop_atoi:
    cmp byte [ebx+esi], 0
    je __end_atoi

    mov ecx, 0
    mov cl, [ebx+esi]
    sub cl, 030h

    mov edx, 10
    mul edx
    jo __fatal_error

    add eax, ecx
    inc esi
    jmp __loop_atoi
    
__end_atoi:
    pop ecx
    pop esi
    pop ebx
    leave
    ret

; int _itoa(int, char*)
_itoa:
    enter 4,0
    push ebx
    push esi

    mov ebx, [ebp+12]
    mov esi, 0

    mov eax, [ebp+8]

__loop_itoa:
    cdq
    mov ecx, 10
    div ecx

    add dl, 0x30

    mov byte [temp_str+esi], dl
    inc esi

    cmp eax, 0
    jne __loop_itoa

    mov ecx, esi
    mov esi, 0

__loop_revert_str_itoa:
    mov byte al, [temp_str+ecx-1]
    mov byte [ebx+esi], al
    inc esi
    loop __loop_revert_str_itoa

    mov eax, esi
    pop esi
    pop ebx

    leave
    ret

__fatal_error:
    push dword error_msg
    push dword error_msg_length
    call _puts

    mov eax, 1
    mov ebx, 1
    int 0x80


section .data
overflow_msg db 'Overflow detected', 0
__mem_28 dd 2
__mem_29 dd 0
__mem_30 dd 0
__mem_31 dd 0
section .bss
__input_msg resb 100
section .text
global _start
_start:
push __input_msg
call _reads
call _atoi
mov [__mem_29], eax
pop edx
mov eax, [__mem_29]
__label_4:
idiv dword [__mem_28]
mov [__mem_30], eax
imul dword [__mem_28]
jo __overflow
mov [__mem_31], eax
mov eax, [__mem_29]
sub eax, [__mem_31]
mov [__mem_31], eax
push __input_msg
push dword [__mem_31]
call _itoa
pop edx
push eax
call _puts
pop edx
pop edx

mov eax, [__mem_30]
mov [__mem_29], eax
mov eax, [__mem_29]
cmp eax, 0
jg __label_4
mov eax, 1
mov ebx, 0
int 80h




__overflow:
mov eax, 4
mov ebx, 1
mov ecx, overflow_msg
mov edx, 17
int 80h
mov eax, 1
mov ebx, 0
int 80h
