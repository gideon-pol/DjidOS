extern _ZN16InterruptManager15HandleInterruptEP11int_frame_t

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

%macro PUSH_INTFRAME 0
    push rax
    push rcx
    push rdx
    push rbp
    push rsi
    push rdi
    push r8
    push r9
    push r10
    push r11
%endmacro

%macro POP_INTFRAME 0
    pop r11
    pop r10
    pop r9
    pop r8
    pop rdi
    pop rsi
    pop rbp
    pop rdx
    pop rcx
    pop rax
%endmacro

%macro PUSH_CPUSTATE 0
    PUSH_INTFRAME
    push r12
    push r13
    push r14
    push r15
    push rbx
%endmacro

%macro POP_CPUSTATE 0
    pop rbx
    pop r15
    pop r14
    pop r13
    pop r12
    POP_INTFRAME
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

extern _ZN4Time20HandleTimerInterruptEP9cpu_state
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

global PitHandler
PitHandler:
    cli
    cld

    PUSH_CPUSTATE

    mov rdi, rsp
    mov r12, rsp
    and rsp, -16

    call _ZN4Time20HandleTimerInterruptEP9cpu_state

    mov rdi, r12
    jmp load_cpu_state

global load_cpu_state
load_cpu_state:
    mov rsp, rdi
    POP_CPUSTATE

    iretq

global save_cpu_state
save_cpu_state:
    sub rsp, 20
    PUSH_CPUSTATE
    mov rax, rsp
    retq

isr_wrapper:
    cld
    PUSH_INTFRAME

    mov rdi, rsp
    mov r12, rsp
    and rsp, -16

    call _ZN16InterruptManager15HandleInterruptEP11int_frame_t

    mov rsp, r12
    POP_INTFRAME
    
    add rsp, 8
    iretq
