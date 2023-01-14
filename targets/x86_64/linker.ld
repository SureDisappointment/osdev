ENTRY(start)

SECTIONS
{
    . = 1M;
    .boot ALIGN(8) :
    {
        KEEP(*(.multiboot_header))
    }

    .text :
    {
        *(.text)
        *(.init)
        *(.fini)
    }

    .init_array :
    {
        PROVIDE_HIDDEN(__init_array_start = .);
        KEEP(*(SORT(.init_array.*)))
        KEEP(*(.init_array))
        KEEP(*(.ctors))
        KEEP(*(.ctor))
        PROVIDE_HIDDEN(__init_array_end = .);
    }

    .fini_array :
    {
        PROVIDE_HIDDEN(__fini_array_start = .);
        KEEP(*(SORT(.fini_array.*)))
        KEEP(*(.fini_array))
        KEEP(*(.dtors))
        KEEP(*(.dtor))
        PROVIDE_HIDDEN(__fini_array_end = .);
    }

    .rodata :
    {
        *(.rodata)
    }

    .data :
    {
        *(.data)
        *(.mbi.data)
    }

    .bss ALIGN(16) : 
    {
        ___BSS_START__ = .;
        *(.bss)
        ___BSS_END__ = .;
    }

    .global_pagetable ALIGN(4096) :
    {
        *(.global_pagetable)
    }
}
