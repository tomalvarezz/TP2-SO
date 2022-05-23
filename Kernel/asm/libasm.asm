GLOBAL cpuVendor
GLOBAL RTC
GLOBAL getRSP
GLOBAL callTimerTick
GLOBAL exchange

section .text
	
cpuVendor:
	push rbp
	mov rbp, rsp

	push rbx

	mov rax, 0
	cpuid


	mov [rdi], ebx
	mov [rdi + 4], edx
	mov [rdi + 8], ecx

	mov byte [rdi+13], 0

	mov rax, rdi

	pop rbx

	mov rsp, rbp
	pop rbp
	ret
RTC:
	cli
	mov al, 0x0B
    out 70h, al
    in al, 71h
    or al, 4
    out 71h, al

    mov rax, rdi
    out 70h, al
    in al, 71h
	sti
    ret

getRSP:
	mov rax, rsp
	ret

callTimerTick:
	int 20h
	ret

exchange:
	;guardo 1 en rax, para poder utilizar el rax para xchg
	mov rax, rsi
	;en rdi está lock, [rdi] el valor de lock
	xchg [rdi], rax		
	;como rax se va a devolver, devuelve 1 para acquire
	;devuelve 0 para release
	ret
