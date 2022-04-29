section .text

GLOBAL sys_read
GLOBAL sys_write
GLOBAL sys_time
GLOBAL sys_clear
GLOBAL sys_get_registers
GLOBAL sys_get_memory
GLOBAL sys_sleep
GLOBAL sys_invalid_op_code

;Cabe aclarar que en todas las funciones se realiza el armado y desarmado del stackFrame, el cual no es extrictamente necesario
;estos casos. Sin embargo se realiza por buen estilo
sys_read:
	push rbp
	mov rbp, rsp
	push rbx
	mov rax, 0x00
	int 80h
	pop rbx
	mov rsp, rbp
	pop rbp
	ret

sys_write:
	push rbp
	mov rbp, rsp
	push rbx
	mov rax, 0x01
	int 80h
	pop rbx
	mov rsp, rbp
	pop rbp
	ret

sys_time:
	push rbp
	mov rbp, rsp
	push rbx
	mov rax, 0x02
	int 80h
	pop rbx
	mov rsp, rbp
	pop rbp
	ret

sys_clear:
	push rbp
	mov rbp, rsp
	push rbx
	mov rax, 0x03
	int 80h
	pop rbx
	mov rsp, rbp
	pop rbp
	ret

sys_get_registers:
	push rbp
	mov rbp, rsp
	push rbx
	mov rax, 0x04
	int 80h
	pop rbx
	mov rsp, rbp
	pop rbp
	ret

sys_get_memory:
	push rbp
	mov rbp, rsp
	push rbx
	mov rax, 0x05
	int 80h
	pop rbx
	mov rsp, rbp
	pop rbp
	ret

sys_sleep:
	push rbp
	mov rbp, rsp
	push rbx
	mov rax, 0x09
	int 80h
	pop rbx
	mov rsp, rbp
	pop rbp
	ret

sys_invalid_op_code:
	UD2
	ret