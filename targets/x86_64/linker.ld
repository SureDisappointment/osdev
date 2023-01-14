OUTPUT_FORMAT("elf64-x86-64")
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
        *(.head.text)
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

    .data ALIGN(16) :
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

    .rodata :
    {
        *(.rodata)
    }

    .global_pagetable ALIGN(4096) :
    {
        *(.global_pagetable)
    }
}
