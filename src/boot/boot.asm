global start

%define KERNEL_OFFSET 0xFFFFFF8000000000
%define V2P(a) ((a) - KERNEL_OFFSET)
%define P2V(a) ((a) + KERNEL_OFFSET)

section .text
bits 32
start:
    ;call check_multiboot
    ;call check_cpuid
    mov dword esp, V2P(stack_top)
    push ebx
    call setup_paging
    pop ebx
    jmp switch_long_mode

setup_paging:
    mov eax, V2P(p3_table)
    or eax, 0b11
    %if 1
    mov dword [V2P(p4_table)], eax 
    %endif
    mov dword [V2P(p4_table) + 4088], eax

    mov eax, V2P(p2_table)
    or eax, 0b11
    mov dword [V2P(p3_table) + 0], eax

    mov ecx, 0
    .p2_table_loop:
    mov eax, 0x200000
    mul ecx
    or eax, 0b10000011
    mov [V2P(p2_table) + ecx * 8], eax

    inc ecx
    cmp ecx, 512
    jne .p2_table_loop

    mov eax, V2P(p4_table)
    mov cr3, eax

    mov eax, cr4
    or eax, 1 << 5
    mov cr4, eax

    mov ecx, 0xc0000080
    rdmsr
    or eax, 1 << 8
    wrmsr

    mov eax, cr0
    or eax, 1 << 31
    or eax, 1 << 16
    mov cr0, eax

    lgdt [V2P(gdt64.pointer)]

    ret

switch_long_mode:
    mov ax, gdt64.data
    mov ss, ax
    mov ds, ax
    mov es, ax

    jmp gdt64.code:V2P(upper_half_jump_64)

bits 64
extern prekernel
upper_half_jump_64:
    mov rax, KERNEL_OFFSET
    add rsp, rax

    mov rax, gdt64.pointer
    lgdt [rax]
    mov rax, 0x0
    mov ss, rax
    mov ds, rax
    mov es, rax

    mov rax, p4_table
    %if 0 mov qword [rax], 0 
    %endif

    mov rdi, rbx

    mov rax, prekernel
    call rax
    hlt

align 4096
p4_table:
    resb 4096
p3_table:
    resb 4096
p2_table:
    resb 4096

section .bss
stack_bottom:
    resb 4096 * 16
stack_top:

section .rodata
gdt64:
    dq 0
    .code: equ $-gdt64
        dq (1 << 44) | (1 << 47) | (1 << 41) | (1 << 43) | (1 << 53)
    .data: equ $-gdt64
        dq (1 << 44) | (1 << 47) | (1 << 41)
    .pointer
        dw .pointer - gdt64 - 1
        dq gdt64