ENTRY(bootstrap)

KERNEL_PMA = 0x00100000;
KERNEL_VMA = 0xE0000000 + KERNEL_PMA + 4096;

SECTIONS
{
    . = KERNEL_VMA;
    _kernel_start = .;
    . = ALIGN(4096);

    .text BLOCK(4096) : ALIGN(4K)
    {
        *bootstrap*(.text)
        *(.text)
        *(.gnu.linkonce.t*)
    }
    .rodata BLOCK(4096) : ALIGN(4K)
    {
        ctor_start = .;
       *(SORT(.ctors*))
        ctor_end = .;
        dtor_start = .;
        *(SORT(.dtors*))
        dtor_end = .;

        *(.rodata*)
        *(.gnu.linkonce.r*)
    }
    .data BLOCK(4096) : ALIGN(4K)
    {
        *(.data)
        *(.gnu.linkonce.d*)
    }
    .bss BLOCK(4096) : ALIGN(4K)
    {
        sbss = .;
        *(.bss)
        *(COMMON)
        *(.gnu.linkonce.b*)
        ebss = .;
    }
    _kernel_end = .;

    /DISCARD/ :
    {
        *(.comment)
        *(.eh_frame*)
    }
}

