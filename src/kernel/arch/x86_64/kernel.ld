OUTPUT_FORMAT(elf64-x86-64)
ENTRY(bootstrap)

KERNEL_PMA = 0x00100000;
KERNEL_VMA = 0xFFFFFFFF80000000 + KERNEL_PMA +4096;

SECTIONS
{
    . = KERNEL_VMA;
    _kernel_start = .;
    . = ALIGN(4096);

    .text BLOCK(4096) :
    {
        *bootstrap*(.text)
        *(.text)
        *(.gnu.linkonce.t*)
        . = ALIGN(4096);
    }
    .rodata BLOCK(4096) :
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
    .data BLOCK(4096) :
    {
        *(.data)
        *(.gnu.linkonce.d*)
    }
    .bss BLOCK(4096) :
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

