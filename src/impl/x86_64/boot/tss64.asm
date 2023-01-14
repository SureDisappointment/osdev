global tss64
global tss64.length
global set_kernel_stack

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