global outb
global outw
global inb
global inw

section .text
; void outb(int port, int value)
outb:
	push   rbp
	mov    rbp, rsp
	mov    rdx, rdi
	mov    rax, rsi
	out    dx, al
	pop    rbp
	ret

; void outw(int port, int value)
outw:
	push   rbp
	mov    rbp, rsp
	mov    rdx, rdi
	mov    rax, rsi
	out    dx, ax
	pop    rbp
	ret

; unsigned char inb(int port)
inb:
	push   rbp
	mov    rbp, rsp
	mov    rdx, rdi
	in     al, dx
	pop    rbp
	ret

; unsigned short inw(int port)
inw:
	push   rbp
	mov    rbp, rsp
	mov    rdx, rdi
	in     ax, dx
	pop    rbp
	ret
