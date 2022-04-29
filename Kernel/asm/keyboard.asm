section .text

GLOBAL bring_key

bring_key:
	push rbp
	mov rbp, rsp

	mov rax, 0
	in al,0x60

	mov rsp, rbp
	pop rbp
	ret