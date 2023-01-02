global long_mode_start

; C kernel functions
extern kmain
extern guardian

; compiler supplied addresses
extern ___BSS_START__
extern ___BSS_END__
extern __init_array_start
extern __init_array_end
extern __fini_array_start
extern __fini_array_end


section .text
bits 64
long_mode_start:
    ; load null into all data segment registers
    mov ax, 0
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

    call setup_idt
    call reprogram_pics ; reprogram PICs so that all 15 hardware interrupts sit sequentially in the idt.
    call setup_cursor ; restore blinking cursor (switched off by GRUB)

    fninit ; activate FPU
    call _init ; call global constructors
    call kmain ; start kernel
    call _fini ; call global destructors
    cli
.hlt:
    hlt
    jmp .hlt


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

    ; ... used by the wrapper
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

setup_cursor:
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
	ret

reprogram_pics:
	mov    al, 0x11   ; ICW1: 8086-mode with ICW4
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
	mov    al, 0x04   ; ICW3 Master: Slaves at IRQs
	out    0x21, al
	call   delay
	mov    al, 0x02   ; ICW3 Slave: Connected to IRQ2 of Master
	out    0xa1, al
	call   delay
	mov    al, 0x03   ; ICW4: 8086-mode and automatic EOI
	out    0x21, al
	call   delay
	out    0xa1, al
	call   delay

	mov    al, 0xff   ; mask hardware interrupts by PICs.
	out    0xa1, al   ; only interrupt 2, which is used
	call   delay      ; to cascade the two PICs, is allowed.
	mov    al, 0xfb
	out    0x21, al

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
	jmp    .L2
.L2:
	ret


section .data

;
; interrupt descriptor table with 256 entries
;
idt:
%macro idt_entry 1
	dw  (wrapper_%1 - wrapper_0) & 0xffff ; offset 0 .. 15
	dw  0x0000 | 0x8 * 2 ; selector points to 64-bit codesegment descriptor of GDT
	dw  0x8e00 ; 8 -> interrupt is present, e -> 80386 32-bit interrupt gate
	dw  ((wrapper_%1 - wrapper_0) & 0xffff0000) >> 16 ; offset 16 .. 31
	dd  ((wrapper_%1 - wrapper_0) & 0xffffffff00000000) >> 32 ; offset 32..63
	dd  0x00000000 ; reserved
%endmacro

%assign i 0
%rep 256
idt_entry i
%assign i i+1
%endrep

idt_descr:
	dw  256*8 - 1    ; 256 entries
	dq idt
