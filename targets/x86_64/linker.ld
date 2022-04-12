ENTRY(start)

SECTIONS
{
    . = 1M;

    .boot :
    {
        KEEP(*(.boot_header))
    }

    text :
    {
        *(.text)
    }
}