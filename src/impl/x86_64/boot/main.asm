; functions
global start

; gdt 
extern gdt64
extern gdt64.kernel_code
extern gdt64.tss
extern gdt64.tss.descriptor
extern gdt64.pointer

; tss
extern tss64

; multiboot information
extern mbi_addr
extern mb_magic

; long mode entry point
extern long_mode_start

section .text
bits 32
start:
    mov esp, stack_top

    ; save grub multiboot info
    mov DWORD [mb_magic], eax
    mov DWORD [mbi_addr], ebx

    call check_multiboot
    call check_cpuid
    call check_long_mode
    call check_A20 ; GRUB should enable the A20 line

    call setup_page_tables
    call enable_paging

    lgdt [gdt64.pointer] ; load gdt

    call init_tss
    mov ax, gdt64.tss
    ltr ax ; load tss

    jmp gdt64.kernel_code:long_mode_start ; reload cs register, activating gdt and jumping to 64-bit code

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

check_A20:
    mov ax, 0
.check_A20_s:
    pushad
    mov edi,0x112345  ;odd megabyte address.
    mov esi,0x012345  ;even megabyte address.
    mov [esi], esi    ;making sure that both addresses contain diffrent values.
    mov [edi], edi    ;(if A20 line is cleared the two pointers would point to the address 0x012345 that would contain 0x112345 (edi)) 
    cmpsd             ;compare addresses to see if the're equivalent.
    popad
    je .A20_off_1     ;if equivalent, the A20 line is cleared.
    ret               ;if not equivalent, A20 line is set.
.A20_off_1:
    cmp ax, 1
    je .A20_off_2
    cmp ax, 2
    je .no_A20
    call enable_A20_kbd
    mov ax, 1
    jmp .check_A20_s
.A20_off_2:
    in al, 0x92
    test al, 2
    jnz .A20_off_2_after
    or al, 2
    and al, 0xFE
    out 0x92, al
.A20_off_2_after:
    mov ax, 2
    jmp .check_A20_s
.no_A20:
    mov al, "A"
    jmp error

enable_A20_kbd:
        cli

        call    .a20wait
        mov     al,0xAD
        out     0x64,al

        call    .a20wait
        mov     al,0xD0
        out     0x64,al

        call    .a20wait2
        in      al,0x60
        push    eax

        call    .a20wait
        mov     al,0xD1
        out     0x64,al

        call    .a20wait
        pop     eax
        or      al,2
        out     0x60,al

        call    .a20wait
        mov     al,0xAE
        out     0x64,al

        call    .a20wait
        sti
        ret
.a20wait:
        in      al,0x64
        test    al,2
        jnz     .a20wait
        ret
.a20wait2:
        in      al,0x64
        test    al,1
        jz      .a20wait2
        ret

setup_page_tables:
    ; disable paging
    mov eax, cr0                                   ; Set the A-register to control register 0
    and eax, 01111111111111111111111111111111b     ; Clear the PG-bit, which is bit 31
    mov cr0, eax                                   ; Set control register 0 to the A-register

    ; clear tables, pass page table location to cpu
    mov edi, page_table_l4       ; Set the destination index.
    mov cr3, edi                 ; Set control register 3 to the destination index.
    xor eax, eax                 ; Nullify the A-register.
    mov ecx, 4096                ; Set the C-register to 4096.
    rep stosd                    ; Clear the memory.
    mov edi, cr3                 ; Set the destination index to control register 3.

    mov eax, page_table_l3
    or eax, 0b111                ; present, writable, usermode-accessible
    mov [page_table_l4], eax

    mov eax, page_table_l2
    or eax, 0b111                ; present, writable, usermode-accessible
    mov [page_table_l3], eax

    mov eax, page_table_l1
    or eax, 0b111                ; present, writable, usermode-accessible
    mov [page_table_l2], eax

; Identity map first 2 MiB kernel memory
    mov edi, page_table_l1
    mov ebx, 0b111                ; present, writable
    mov ecx, 512
.SetEntry1:
    mov DWORD [edi], ebx         ; Set the uint32_t at the destination index to the B-register.
    add ebx, 0x1000              ; Add 0x1000 to the B-register.
    add edi, 8                   ; Add eight to the destination index.
    loop .SetEntry1              ; Set the next entry.

; Map video memory (0xb8000 - 0xbffff) to 0x2b8000 for userspace
    mov eax, page_table_l1 + 4096
    or eax, 0b111                ; present, writable, usermode-accessible
    mov [page_table_l2 + 8], eax

    mov edi, page_table_l1
    add edi, 5568
    mov ebx, 0xb8000
    or ebx, 0b111                ; present, writable, usermode-accessible
    mov ecx, 8
.SetEntry2:
    mov DWORD [edi], ebx         ; Set the uint32_t at the destination index to the B-register.
    add ebx, 0x1000              ; Add 0x1000 to the B-register.
    add edi, 8                   ; Add eight to the destination index.
    loop .SetEntry2              ; Set the next entry.

    ret

enable_paging:
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

init_tss:
    mov	edi, tss64
    ; RSP0 (lower 32 bits)
    mov dword [edi+4], stack_top

    ; update gdt entry
    mov	edi,			gdt64.tss.descriptor
    mov	eax,			tss64
    ; Set Base Low [15:00]
    mov	[edi+2],		ax
    shr	eax,			16
    ; Set Base Middle [23:16]
    mov	[edi+4],		al
    shr	eax,			8
    ; Set Base High [31:24]
    mov	[edi+7],		al

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
page_table_l1:
    resb 4096*2
