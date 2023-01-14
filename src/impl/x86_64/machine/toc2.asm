absolute 0
rbx_offset: resq 1
r12_offset: resq 1
r13_offset: resq 1
r14_offset: resq 1
r15_offset: resq 1
rbp_offset: resq 1
rsp_offset: resq 1
fpu_offset: resb 108

global toc_switch
global toc_go

section .text
toc_go:
    mov rbx, [rdi+rbx_offset]
    mov r12, [rdi+r12_offset]
    mov r13, [rdi+r13_offset]
    mov r14, [rdi+r14_offset]
    mov r15, [rdi+r15_offset]
    mov rbp, [rdi+rbp_offset]
    mov rsp, [rdi+rsp_offset]
    ret

toc_switch:
    mov [rdi+rbx_offset], rbx
    mov [rdi+r12_offset], r12
    mov [rdi+r13_offset], r13
    mov [rdi+r14_offset], r14
    mov [rdi+r15_offset], r15
    mov [rdi+rbp_offset], rbp
    mov [rdi+rsp_offset], rsp
    mov rbx, [rsi+rbx_offset]
    mov r12, [rsi+r12_offset]
    mov r13, [rsi+r13_offset]
    mov r14, [rsi+r14_offset]
    mov r15, [rsi+r15_offset]
    mov rbp, [rsi+rbp_offset]
    mov rsp, [rsi+rsp_offset]
    ret
