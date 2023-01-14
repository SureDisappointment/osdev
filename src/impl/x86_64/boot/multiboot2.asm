section .multiboot_header progbits alloc noexec write align=4
header_start:
    ; magic number
    dd 0xe85250d6 ; multiboot2
    ; architecture
    dd 0 ; protected mode i386
    ; header length
    dd header_end - header_start
    ; checksum
    dd 0x100000000 - (0xe85250d6 + 0 + (header_end - header_start))

    ; end tag
    dw 0
    dw 0
    dd 8
header_end:

; space for multiboot information
section .mbi.data progbits alloc noexec write align=4
global mb_magic
global mbi_addr
mb_magic:
    dd 0
mbi_addr:
    dd 0
