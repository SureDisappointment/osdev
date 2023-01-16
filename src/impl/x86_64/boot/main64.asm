global long_mode_start
global jump_usermode

; page tables
extern page_table
extern page_table.l4
extern page_table.l3
extern page_table.l2
extern page_table.l1
extern page_table.length

; gdt segments
extern gdt64.kernel_code
extern gdt64.kernel_data
extern gdt64.user_code
extern gdt64.user_data

; multiboot information
extern mb_magic
extern mbi_addr

; C kernel functions
extern check_multiboot2
extern kmain
extern guardian

; linker supplied addresses
extern ___BSS_START__
extern ___BSS_END__
extern __init_array_start
extern __init_array_end
extern __fini_array_start
extern __fini_array_end


section .head.text progbits alloc exec nowrite align=16
bits 64
long_mode_start:
    ; reload data segment registers
    mov ax, gdt64.kernel_data
    mov ss, ax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; clear BSS
	mov    rdi, ___BSS_START__
.clear_bss:
	mov    byte [rdi], 0
	inc    rdi
	cmp    rdi, ___BSS_END__
	jne    .clear_bss

    call setup_idt ; fill IDT with guardian calls
    call remap_pics ; remap PICs to not conflict with CPU exceptions and mask all PIC lines (for now)
    call setup_cursor ; restore blinking cursor (switched off by GRUB)

    fninit ; activate FPU
    call _init ; call global constructors

	xor rsi, rsi
	xor rdi, rdi
	mov esi, DWORD [mbi_addr] ; get GRUB multiboot info
	mov edi, DWORD [mb_magic] ; get multiboot magic number
	call check_multiboot2

	; unmap identity paging
	mov rax, 0
    mov [page_table.l2], rax
	mov rcx, cr3 ; reload cr3 to force a TLB flush so the changes take effect
	mov cr3, rcx
	mov rsp, stack_top ; load new stack

	xor rbp, rbp ; setup a null stack frame as an anchor for stack backtracing
    call kmain ; start kernel

	; kmain shouldn't return!
	; if - for whatever reason - it does, perform cleanup tasks

	; remap identity paging TODO: obviously doesn't work because page_table isn't identity mapped anymore!
	mov rax, page_table.l1
    or rax, 0b111
    mov [page_table.l2], rax

    call _fini ; call global destructors
    cli ; disable interrupts
.hlt:
    hlt
    jmp .hlt

; void jump_usermode(uint64_t start_addr)
jump_usermode:
	mov ax, gdt64.user_data
	or ax, 3 ; usermode data segment with bottom 2 bits set for ring 3
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax ; SS is handled by iretq

	; set up the stack frame iretq expects
	mov rax, rsp
	mov rbx, gdt64.user_data
	or rbx, 3
	mov rcx, gdt64.user_code
	or rcx, 3
	push rbx ; data selector
	push rax ; stack pointer
	pushf ; rflags
	push rcx ; code selector
	push rdi ; instruction address (of ring 3 function) to jump to

	iretq


; interrupt handler head
%macro wrapper 1
wrapper_%1:
	push   rbp
	mov    rbp, rsp
	push   rax
	mov    al, %1
	jmp    wrapper_body
%endmacro

%assign i 0
%rep 256
wrapper i
%assign i i+1
%endrep

; interrupt handler body
wrapper_body:
	; expected by gcc
	cld
	; preserve volatile registers
	push   rcx
	push   rdx
	push   rdi
	push   rsi
	push   r8
	push   r9
	push   r10
	push   r11

	; generated wrapper provides only 8 bits
	and    rax, 0xff

	; pass interrupt number as argument
	mov    rdi, rax
	; pass location of error code as argument (might not be valid if no error code exists)
	mov    rsi, rbp
	add    rsi, 8
	call   guardian

	; restore volatile registers
	pop    r11
	pop    r10
	pop    r9
	pop    r8
	pop    rsi
	pop    rdi
	pop    rdx
	pop    rcx

    ; pushed by wrapper
	pop    rax
	pop    rbp

	iretq

setup_idt:
	mov    rax, wrapper_0

	; bits 0..15 -> ax, 16..31 -> bx, 32..64 -> edx
	mov    rbx, rax
	mov    rdx, rax
	shr    rdx, 32
	shr    rbx, 16

	mov    r10, idt   ; pointer to current interrupt gate
	mov    rcx, 255   ; counter
.loop:
	add    [r10+0], ax
	adc    [r10+6], bx
	adc    [r10+8], edx
	add    r10, 16
	dec    rcx
	jge    .loop

	lidt   [idt_descr]
	ret

remap_pics:
	mov    al, 0x11   ; ICW1: ICW4 required
	out    0x20, al
	call   delay
	out    0xa0, al
	call   delay
	mov    al, 0x20   ; ICW2 Master: IRQ # Offset (32)
	out    0x21, al
	call   delay
	mov    al, 0x28   ; ICW2 Slave: IRQ # Offset (40)
	out    0xa1, al
	call   delay
	mov    al, 0x04   ; ICW3 Master: Slave at IRQ2
	out    0x21, al
	call   delay
	mov    al, 0x02   ; ICW3 Slave: Cascade identity 0000 0010
	out    0xa1, al
	call   delay
	mov    al, 0x03   ; ICW4: 8086-mode and automatic EOI
	out    0x21, al
	call   delay
	out    0xa1, al
	call   delay

	mov    al, 0xff   ; mask hardware interrupts by PICs.
	out    0xa1, al   ; only interrupt 2, which is used
	call   delay      ; to cascade the two PICs, is allowed
	mov    al, 0xfb   ; for now.
	out    0x21, al

	ret

setup_cursor:
	; enable cursor
	mov al, 0x0a
	mov dx, 0x3d4
	out dx, al
	call delay
	mov dx, 0x3d5
	in al, dx
	call delay
	and al, 0xc0
	or al, 14
	out dx, al
	call delay
	mov al, 0x0b
	mov dx, 0x3d4
	out dx, al
	call delay
	mov dx, 0x3d5
	in al, dx
	call delay
	and al, 0xe0
	or al, 15
	out dx, al
	call delay

	; set cursor position to (0,0)
	mov al, 0x0e
	mov dx, 0x3d4
	out dx, al
	call delay
	mov al, 0x0
	mov dx, 0x3d5
	out dx, al
	call delay
	mov al, 0x0f
	mov dx, 0x3d4
	out dx, al
	call delay
	mov al, 0x0
	mov dx, 0x3d5
	out dx, al

	ret

_init:
	mov    rbx, __init_array_start
_init_loop:
	cmp    rbx, __init_array_end
	je     _init_done
	mov    rax, [rbx]
	call   rax
	add    rbx, 8
	ja     _init_loop
_init_done:
	ret

_fini:
	mov    rbx, __fini_array_start
_fini_loop:
	cmp    rbx, __fini_array_end
	je     _fini_done
	mov    rax, [rbx]
	call   rax
	add    rbx, 8
	ja     _fini_loop
_fini_done:
	ret

;
; short delay for in/out-commands
;
delay:
    mov    al, 0
	out 0x80, al
	ret


section .idt.rodata progbits alloc noexec nowrite align=4
;
; interrupt descriptor table with 256 entries
;
idt:
%macro idt_interrupt_entry 1
	dw  (wrapper_%1 - wrapper_0) & 0xffff ; offset 0 .. 15
	dw  gdt64.kernel_code ; segment selector
	dw  0x8e00 ; 64-bit interrupt gate, present
	dw  ((wrapper_%1 - wrapper_0) & 0xffff0000) >> 16 ; offset 16 .. 31
	dd  ((wrapper_%1 - wrapper_0) & 0xffffffff00000000) >> 32 ; offset 32..63
	dd  0x00000000 ; reserved
%endmacro
%macro idt_trap_entry 1
	dw  (wrapper_%1 - wrapper_0) & 0xffff ; offset 0 .. 15
	dw  gdt64.kernel_code ; segment selector
	dw  0x8f00 ; 64-bit trap gate, present
	dw  ((wrapper_%1 - wrapper_0) & 0xffff0000) >> 16 ; offset 16 .. 31
	dd  ((wrapper_%1 - wrapper_0) & 0xffffffff00000000) >> 32 ; offset 32..63
	dd  0x00000000 ; reserved
%endmacro

; first 32 vectors are reserved for exceptions (as mandated by Intel)
%assign i 0
%rep 32
idt_trap_entry i
%assign i i+1
%endrep
%rep 224
idt_interrupt_entry i
%assign i i+1
%endrep

idt_descr:
	dw  256*8 - 1 ; 256 entries
	dq idt


section .kernel_stack nobits alloc noexec write align=4
; reserve memory for stack
align 16
stack_bottom:
    resb 16384 ; 16 KiB
stack_top:
