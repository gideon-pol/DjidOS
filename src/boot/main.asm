global start_64
extern kernel_main

section .bss
stack_bottom:
    resb 4096 * 16
stack_top:

section .text
bits 64
start_64:
    mov qword rsp, stack_top

    call kernel_main
    hlt