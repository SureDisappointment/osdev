global gdt64
global gdt64.kernel_code
global gdt64.kernel_data
global gdt64.user_code
global gdt64.user_data
global gdt64.tss
global gdt64.tss.descriptor
global gdt64.pointer

global tss64
global tss64.length
global set_kernel_stack

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
.tss.descriptor:
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

section .data
; task state segment
tss64:
    dd 0 ; reserved
    dq 0 ; rsp0 - The stack pointer to load when changing to kernel mode (mode 0).
    dq 0 ; rsp1
    dq 0 ; rsp2
    dq 0 ; reserved
    dq 0 ; ist1 - The stack pointer to load if an idt entry has an ist value of 1
	dq 0 ; ist2
    dq 0 ; ist3
    dq 0 ; ist4
    dq 0 ; ist5
    dq 0 ; ist6
    dq 0 ; ist7
    dq 0 ; reserved
    dw 0 ; reserved
    dw 0 ; iopb - I/O Map Base Address
.length: equ $ - tss64 - 1 ; needed for gdt entry

section .text
bits 64
; void set_kernel_stack(uint64_t stackptr)
set_kernel_stack:
    mov	rsi, tss64
    mov qword[rsi+4], rdi
    ret
