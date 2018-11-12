ENTRY(bootstrap)


KERNEL_PMA = 0x00100000;
KERNEL_VMA = 0xE0000000 + KERNEL_PMA +4096;


SECTIONS
{
    . = KERNEL_VMA;
    _kernel_start = .;
    . = ALIGN(4096);
    
    .text : AT(ADDR(.text) - KERNEL_VMA)
    {
        *bootstrap*(.text)
        *(.text)
        *(.gnu.linkonce.t*)
        . = ALIGN(4096);
    }
    
    .rodata : AT(ADDR(.rodata) - KERNEL_VMA)
    {
        ctor_start = .;
       *(SORT(.ctors*))
        ctor_end = .;
        dtor_start = .;
        *(SORT(.dtors*))
        dtor_end = .;

        *(.rodata*)
        *(.gnu.linkonce.r*)
        . = ALIGN(4096);
    }
    

    .data : AT(ADDR(.data) - KERNEL_VMA)
    {
        *(.data)
        *(.gnu.linkonce.d*)
        . = ALIGN(4096);
    }

    .bss : AT(ADDR(.bss) - KERNEL_VMA)
    {
        sbss = .;
        *(.bss)
        *(COMMON)
        *(.gnu.linkonce.b*)
        ebss = .;
        . = ALIGN(4096);
    }

    _kernel_end = .;

    /DISCARD/ :
    {
        *(.comment)
        *(.eh_frame*)
    }
}

