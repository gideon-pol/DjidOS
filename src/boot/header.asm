bits 32
section .boot_header
header_start:
    dd 0xe85250d6
    dd 0
    dd header_end - header_start
    dd 0x100000000 - (0xe85250d6 + 0 + (header_end - header_start))
%if 0
framebuffer_tag_start:
align 8
    dw 5
    dw 0
    dd framebuffer_tag_end - framebuffer_tag_start
    dd 400
    dd 300
    dd 32
framebuffer_tag_end:
%endif
align 8
    dw 0
    dw 0
    dd 8
header_end:
