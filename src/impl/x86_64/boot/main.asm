global start
extern long_mode_start

section .text
bits 32
start:
    mov esp, stack_top

    call check_multiboot
    call check_cpuid
    call check_long_mode

    call setup_page_tables
    call enable_paging

    lgdt [gdt64.descriptor] ; load gdt
    jmp gdt64.kernel_code_segment:long_mode_start ; reload cs register, activating gdt and jumping to 64-bit code

    hlt

check_multiboot:
    cmp eax, 0x36d76289
    jne .no_multiboot
    ret
.no_multiboot:
    mov al, "M"
    jmp error

check_cpuid:
    pushfd
    pop eax
    mov ecx, eax
    xor eax, 1 << 21
    push eax
    popfd
    pushfd
    pop eax
    push ecx
    popfd
    cmp eax, ecx
    je .no_cpuid
    ret
.no_cpuid:
    mov al, "C"
    jmp error

check_long_mode:
    mov eax, 0x80000000
    cpuid
    cmp eax, 0x80000001
    jb .no_long_mode

    mov eax, 0x80000001
    cpuid
    test edx, 1 << 29
    jz .no_long_mode
    ret
.no_long_mode:
    mov al, "L"
    jmp error

setup_page_tables:
    mov eax, page_table_l3
    or eax, 0b11 ; present, writable
    mov [page_table_l4], eax

    mov eax, page_table_l2
    or eax, 0b11 ; present, writable
    mov [page_table_l3], eax

    mov ecx, 0 ; counter
.loop:
    mov eax, 0x200000 ; 2MiB
    mul ecx
    or eax, 0b10000011 ; present, writable, huge page
    mov [page_table_l2 + ecx * 8], eax

    inc ecx ; increment counter
    cmp ecx, 512 ; checks if the whole table is mapped
    jne .loop ; if not, continue

ret

enable_paging:
    ; pass page table location to cpu
    mov eax, page_table_l4
    mov cr3, eax

    ; enable PAE
    mov eax, cr4
    or eax, 1 << 5
    mov cr4, eax

    ; enable long mode
    mov ecx, 0xC0000080
    rdmsr
    or eax, 1 << 8
    wrmsr

    ; enable paging
    mov eax, cr0
    or eax, 1 << 31
    mov cr0, eax

    ret

error:
    ; print "ERR: X" where X is the error code
    mov dword [0xb8000], 0x4f524f45
    mov dword [0xb8004], 0x4f344f52
    mov dword [0xb8008], 0x4f204f20
    mov byte [0xb800a], al
    hlt

section .bss
; reserve memory for stack
align 16
stack_bottom:
    resb 16384 ; 16 KiB
stack_top:

section .global_pagetable
; reserve memory for page tables
page_table_l4:
    resb 4096 ; 4 KiB
page_table_l3:
    resb 4096
page_table_l2:
    resb 4096

section .rodata
; global descriptor table
gdt64:
    dq 0 ; null descriptor
.kernel_code_segment: equ $ - gdt64 ; offset inside gdt
    dw 0xFFFF ; limit
    db 0, 0, 0 ; base
    db 0x9A ; access byte
    db 0xAF ; limit & flags
    db 0 ; base
.kernel_data_segment: equ $ - gdt64
    dw 0xFFFF ; limit
    db 0, 0, 0 ; base
    db 0x92 ; access byte
    db 0xCF ; limit & flags
    db 0 ; base
.user_code_segment: equ $ - gdt64
    dw 0xFFFF ; limit
    db 0, 0, 0 ; base
    db 0xFA ; access byte
    db 0xAF ; limit & flags
    db 0 ; base
.user_data_segment: equ $ - gdt64
    dw 0xFFFF ; limit
    db 0, 0, 0 ; base
    db 0xF2 ; access byte
    db 0xCF ; limit & flags
    db 0 ; base
.task_state_segment: equ $ - gdt64
    dw 0xFFFF ; limit
    db 0, 0, 0 ; base
    db 0x89 ; access byte
    db 0x0F ; limit & flags
    db 0 ; base
    dq 0 ; base
.descriptor:
    dw $ - gdt64 - 1 ; gtd length
    dq gdt64 ; gdt location
