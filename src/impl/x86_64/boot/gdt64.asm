global gdt64
global gdt64.kernel_code
global gdt64.kernel_data
global gdt64.user_code
global gdt64.user_data
global gdt64.tss

section .rodata
; global descriptor table
gdt64:
    dq 0 ; null descriptor
.kernel_code: equ $ - gdt64 ; offset inside gdt
    dw 0xFFFF ; limit
    db 0, 0, 0 ; base
    db 0x9A ; access byte
    db 0xAF ; limit & flags
    db 0 ; base
.kernel_data: equ $ - gdt64
    dw 0xFFFF ; limit
    db 0, 0, 0 ; base
    db 0x92 ; access byte
    db 0xCF ; limit & flags
    db 0 ; base
.user_code: equ $ - gdt64
    dw 0xFFFF ; limit
    db 0, 0, 0 ; base
    db 0xFA ; access byte
    db 0xAF ; limit & flags
    db 0 ; base
.user_data: equ $ - gdt64
    dw 0xFFFF ; limit
    db 0, 0, 0 ; base
    db 0xF2 ; access byte
    db 0xCF ; limit & flags
    db 0 ; base
.tss: equ $ - gdt64
    dw tss64.length & 0xFFFF ; limit
    dw 0 ; base
    db 0 ; base
    db 0x89 ; access byte
    db (tss64.length & 0xF0000) >> 16 ; limit & flags
    db 0 ; base
    dd 0 ; base
    dd 0 ; reserved
.pointer:
    dw $ - gdt64 - 1 ; gtd length
    dq gdt64 ; gdt location