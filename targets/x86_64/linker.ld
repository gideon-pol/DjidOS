ENTRY(start)

KERNEL_OFFSET = 0xFFFFFF8000000000;
KERNEL_START = 0x10000;

SECTIONS
{   
    .boot :
    {
        KEEP(*(.boot_header))
    }

    . = KERNEL_START + KERNEL_OFFSET;

    _KernelStart = .;
    .text : AT(ADDR(.text) - KERNEL_OFFSET)
    {
        *(.text .text.*)
    }
    .bss : AT(ADDR(.bss) - KERNEL_OFFSET)
    {
        *(.bss .bss.*)
    }
    .rodata : AT(ADDR(.rodata) - KERNEL_OFFSET)
    {
        *(.rodata .rodata.*)
    }
    .init_array : AT(ADDR(.init_array) - KERNEL_OFFSET)
    {
        *(.init_array)
    }
    .comment : AT(ADDR(.comment) - KERNEL_OFFSET)
    {
        *(.comment)
    }
    _KernelEnd = .;
}