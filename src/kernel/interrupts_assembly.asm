extern _ZN16InterruptManager15HandleInterruptEP9int_frame

section .text

%macro ISR_ERR 1
global Handler%+%1:
Handler%+%1:
    cli
    mov dword [rsp-4], %1
    sub rsp, 4
    jmp isr_wrapper
%endmacro

%macro ISR_NOERR 1
global Handler%+%1:
Handler%+%1:
    cli
    mov dword [rsp-4], 0
    mov dword [rsp-8], %1
    sub rsp, 8
    jmp isr_wrapper
%endmacro

%macro PUSHALL 0
    push rax
    push rbx
    push rcx
    push rdx
    push rbp
    push rsi
    push rdi
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15
%endmacro

%macro POPALL 0
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rdi
    pop rsi
    pop rbp
    pop rdx
    pop rcx
    pop rbx
    pop rax
%endmacro

ISR_NOERR 0x0
ISR_NOERR 0x1
ISR_NOERR 0x2
ISR_NOERR 0x3
ISR_NOERR 0x4
ISR_NOERR 0x5
ISR_NOERR 0x6
ISR_NOERR 0x7
ISR_ERR 0x8
ISR_NOERR 0x9
ISR_ERR 0xA
ISR_ERR 0xB
ISR_ERR 0xC
ISR_ERR 0xD
ISR_ERR 0xE
ISR_NOERR 0xF

ISR_NOERR 0x10
ISR_ERR 0x11
ISR_NOERR 0x12
ISR_NOERR 0x13
ISR_NOERR 0x14
ISR_ERR 0x15
ISR_NOERR 0x1C
ISR_NOERR 0x1D
ISR_ERR 0x1E

ISR_NOERR 0x20
ISR_NOERR 0x21
ISR_NOERR 0x22
ISR_NOERR 0x23
ISR_NOERR 0x24
ISR_NOERR 0x25
ISR_NOERR 0x26
ISR_NOERR 0x27
ISR_NOERR 0x28
ISR_NOERR 0x29
ISR_NOERR 0x2A
ISR_NOERR 0x2B
ISR_NOERR 0x2C

isr_wrapper:
    cld
    PUSHALL

    mov rdi, rsp
    mov r12, rsp
    and rsp, -16

    call _ZN16InterruptManager15HandleInterruptEP9int_frame

    mov rsp, r12
    POPALL
    add rsp, 8

    iretq