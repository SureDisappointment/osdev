OUTPUT_FORMAT("elf64-x86-64")
ENTRY(start)

SECTIONS
{
    . = 1M;

    _kernel_start = .;
    .boot ALIGN(8) :
    {
        KEEP(*(.multiboot_header))
        *(.mbi.data)
    }

    .entry.text :
    {
        *(.entry.text)
    }

    .bootstrap_stack :
    {
        *(.bootstrap_stack)
    }

    .idt.rodata :
    {
        *(.idt.rodata)
    }

    .global_pagetable ALIGN(4K) :
    {
        *(.global_pagetable)
    }

    . += 0xC0000000;
    .text ALIGN(4K) : AT(ADDR(.text) - 0xC0000000)
    {
        *(.head.text)
        *(.text)
        *(.init)
        *(.fini)
    }

    .init_array ALIGN(4K) : AT(ADDR(.init_array) - 0xC0000000)
    {
        PROVIDE_HIDDEN(__init_array_start = . - 0xC0000000);
        KEEP(*(SORT(.init_array.*)))
        KEEP(*(.init_array))
        KEEP(*(.ctors))
        KEEP(*(.ctor))
        PROVIDE_HIDDEN(__init_array_end = . - 0xC0000000);
    }

    .fini_array ALIGN(4K) : AT(ADDR(.fini_array) - 0xC0000000)
    {
        PROVIDE_HIDDEN(__fini_array_start = . - 0xC0000000);
        KEEP(*(SORT(.fini_array.*)))
        KEEP(*(.fini_array))
        KEEP(*(.dtors))
        KEEP(*(.dtor))
        PROVIDE_HIDDEN(__fini_array_end = . - 0xC0000000);
    }

    .rodata ALIGN(4K) : AT(ADDR(.rodata) - 0xC0000000)
    {
        *(.rodata)
    }

    .data ALIGN(4K) : AT(ADDR(.data) - 0xC0000000)
    {
        *(.data)
    }

    .bss ALIGN(4K) : AT(ADDR(.bss) - 0xC0000000)
    {
        ___BSS_START__ = . - 0xC0000000;
        *(.bss)
        *(.kernel_stack)
        ___BSS_END__ = . - 0xC0000000;
    }
    _kernel_end = . - 0xC0000000;
    _kernel_start_virt = _kernel_start + 0xC0000000;
    _kernel_end_virt = _kernel_end + 0xC0000000;
}
