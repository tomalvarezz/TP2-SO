section .text

GLOBAL sys_read
GLOBAL sys_write
GLOBAL sys_time
GLOBAL sys_clear
GLOBAL sys_get_registers
GLOBAL sys_get_memory
GLOBAL sys_malloc
GLOBAL sys_free
GLOBAL sys_memory_dump
GLOBAL sys_sleep
GLOBAL sys_new_process
GLOBAL sys_kill_process
GLOBAL sys_block_process
GLOBAL sys_ready_process
GLOBAL sys_get_process_pid
GLOBAL sys_set_priority
GLOBAL sys_processes_status
GLOBAL sys_current_process_status
GLOBAL sys_yield
GLOBAL sys_sem_open
GLOBAL sys_sem_wait
GLOBAL sys_sem_post
GLOBAL sys_sem_close
GLOBAL sys_sem_status
GLOBAL sys_pipe_open
GLOBAL sys_pipe_write
GLOBAL sys_pipe_read
GLOBAL sys_pipe_close
GLOBAL sys_pipe_status


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
sys_malloc:
    push rbp
    mov rbp, rsp
    push rbx
    mov rax, 0x06
    int 80h
    pop rbx
    mov rsp, rbp
    pop rbp
    ret

sys_free:
    push rbp
    mov rbp, rsp
    push rbx
    mov rax, 0x07
    int 80h
    pop rbx
    mov rsp, rbp
    pop rbp
    ret

sys_memory_dump:
    push rbp
    mov rbp, rsp
    push rbx
    mov rax, 0x08
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

sys_new_process:
	push rbp
	mov rbp, rsp
	push rbx
	mov rax, 0x0A
	int 80h
	pop rbx
	mov rsp, rbp
	pop rbp
	ret

sys_kill_process:
	push rbp
	mov rbp, rsp
	push rbx
	mov rax, 0x0B
	int 80h
	pop rbx
	mov rsp, rbp
	pop rbp
	ret

sys_block_process:
	push rbp
	mov rbp, rsp
	push rbx
	mov rax, 0x0C
	int 80h
	pop rbx
	mov rsp, rbp
	pop rbp
	ret

sys_ready_process:
	push rbp
	mov rbp, rsp
	push rbx
	mov rax, 0x0D
	int 80h
	pop rbx
	mov rsp, rbp
	pop rbp
	ret

sys_get_process_pid:
	push rbp
	mov rbp, rsp
	push rbx
	mov rax, 0x0E
	int 80h
	pop rbx
	mov rsp, rbp
	pop rbp
	ret

sys_set_priority:
	push rbp
	mov rbp, rsp
	push rbx
	mov rax, 0x0F
	int 80h
	pop rbx
	mov rsp, rbp
	pop rbp
	ret

sys_processes_status:
	push rbp
	mov rbp, rsp
	push rbx
	mov rax, 0x10
	int 80h
	pop rbx
	mov rsp, rbp
	pop rbp
	ret

sys_current_process_status:
	push rbp
	mov rbp, rsp
	push rbx
	mov rax, 0x11
	int 80h
	pop rbx
	mov rsp, rbp
	pop rbp
	ret

sys_yield:
	push rbp
	mov rbp, rsp
	push rbx
	mov rax, 0x12
	int 80h
	pop rbx
	mov rsp, rbp
	pop rbp
	ret

sys_sem_open:
	push rbp
	mov rbp, rsp
	push rbx
	mov rax, 0x13
	int 80h
	pop rbx
	mov rsp, rbp
	pop rbp
	ret

sys_sem_wait:
	push rbp
	mov rbp, rsp
	push rbx
	mov rax, 0x14
	int 80h
	pop rbx
	mov rsp, rbp
	pop rbp
	ret

sys_sem_post:
	push rbp
	mov rbp, rsp
	push rbx
	mov rax, 0x15
	int 80h
	pop rbx
	mov rsp, rbp
	pop rbp
	ret

sys_sem_close:
	push rbp
	mov rbp, rsp
	push rbx
	mov rax, 0x16
	int 80h
	pop rbx
	mov rsp, rbp
	pop rbp
	ret

sys_sem_status:
	push rbp
	mov rbp, rsp
	push rbx
	mov rax, 0x17
	int 80h
	pop rbx
	mov rsp, rbp
	pop rbp
	ret

sys_pipe_open:
	push rbp
	mov rbp, rsp
	push rbx
	mov rax, 0x18
	int 80h
	pop rbx
	mov rsp, rbp
	pop rbp
	ret

sys_pipe_write:
	push rbp
	mov rbp, rsp
	push rbx
	mov rax, 0x19
	int 80h
	pop rbx
	mov rsp, rbp
	pop rbp
	ret

sys_pipe_read:
	push rbp
	mov rbp, rsp
	push rbx
	mov rax, 0x1A
	int 80h
	pop rbx
	mov rsp, rbp
	pop rbp
	ret

sys_pipe_close:
	push rbp
	mov rbp, rsp
	push rbx
	mov rax, 0x1B
	int 80h
	pop rbx
	mov rsp, rbp
	pop rbp
	ret

sys_pipe_status:
	push rbp
	mov rbp, rsp
	push rbx
	mov rax, 0x1C
	int 80h
	pop rbx
	mov rsp, rbp
	pop rbp
	ret
sys_invalid_op_code:
	UD2
	ret