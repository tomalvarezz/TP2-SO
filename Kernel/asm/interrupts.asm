GLOBAL _cli
GLOBAL _sti
GLOBAL picMasterMask
GLOBAL picSlaveMask
GLOBAL haltcpu
GLOBAL _hlt

GLOBAL _irq00Handler
GLOBAL _irq01Handler
GLOBAL _irq02Handler
GLOBAL _irq03Handler
GLOBAL _irq04Handler
GLOBAL _irq05Handler
GLOBAL _irq80Handler

GLOBAL _exception0Handler
GLOBAL _exception6Handler
GLOBAL get_saved_registers

EXTERN irqDispatcher
EXTERN syscall_handler
EXTERN exceptionDispatcher
EXTERN scheduler

SECTION .text

%macro pushState 0
	push rax
	push rbx
	push rcx
	push rdx
	push rbp
	push rdi
	push rsi
	push r8
	push r9
	push r10
	push r11
	push r12
	push r13
	push r14
	push r15
%endmacro

%macro popState 0
	pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	pop r10
	pop r9
	pop r8
	pop rsi
	pop rdi
	pop rbp
	pop rdx
	pop rcx
	pop rbx
	pop rax
%endmacro

%macro irqHandlerMaster 1
	pushState

	mov rdi, %1 ; pasaje de parametro

	;Si rdi es '1' quiere decir que la interrupción fue de teclado
	;Si no es uno, salto la parte de guardar registros
	cmp rdi, 1
	jnz .continue

	;Levanto el valor de todos los registros previo a entrar a la interrupción
	;El rsp tendrá el valor que le correspondía a antes de entrar 
	popState

	mov qword [regs], r15
	mov qword [regs+8], r14
	mov qword [regs+(2*8)], r13
	mov qword [regs+(3*8)], r12
	mov qword [regs+(4*8)], r11
	mov qword [regs+(5*8)], r10
	mov qword [regs+(6*8)], r9
	mov qword [regs+(7*8)], r8
	mov qword [regs+(8*8)], rsi
	mov qword [regs+(9*8)], rdi
	mov qword [regs+(10*8)], rbp
	mov qword [regs+(11*8)], rdx
	mov qword [regs+(12*8)], rcx
	mov qword [regs+(13*8)], rbx
	mov qword [regs+(14*8)], rax
	mov qword [regs+(15*8)], rsp

	;Vuelvo a mandar los registros al stack para el popState previo a salir de la macro
	pushState

	;Fuerzo el pasaje a rdi de 1 porque en el popState "perdí" el valor inicial de rdi
	;Sabemos que es 1 porque sino hubiera ido al continue
	mov rdi, 1

	.continue:
	call irqDispatcher

	;Después de llamar a la interrupción piso todos los valores en 0
	call delete_saved_registers

	; signal pic EOI (End of Interrupt)
	mov al, 20h
	out 20h, al

	popState
	iretq
%endmacro

;Macro para las irq de software
; %macro irqHandlerMasterSoftware 1
; 	mov rcx, rax ;movemos el contenido de rax a rcx que va a ser el codigo de syscall en caso de int80
; 	mov r8, %1  ;Pasaje de parametro
; 	mov r9, rsp ;pasaje de stackFrame
; 	call irqDispatcherSoftware

; 	iretq
; %endmacro



%macro exceptionHandler 1
	pushState

	mov rdi, %1 ; pasaje de parametro
	mov rsi, rsp ; movemos el stack pointer al comienzo de la pila donde se encuentran todos los registros pusheados 
	call exceptionDispatcher

	popState
	iretq
%endmacro

%macro pushAllState 0
	push rax
	push rbx
	push rcx
	push rdx
	push rbp
	push rdi
	push rsi
	push r8
	push r9
	push r10
	push r11
	push r12
	push r13
	push r14
	push r15
	push fs
    push gs
%endmacro

%macro popAllState 0
	pop gs
	pop fs
	pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	pop r10
	pop r9
	pop r8
	pop rsi
	pop rdi
	pop rbp
	pop rdx
	pop rcx
	pop rbx
	pop rax
%endmacro

_hlt:
	sti
	hlt
	ret

_cli:
	cli
	ret


_sti:
	sti
	ret

picMasterMask:
	push rbp
    mov rbp, rsp
    mov ax, di
    out	21h,al
    pop rbp
    retn

picSlaveMask:
	push    rbp
    mov     rbp, rsp
    mov     ax, di  ; ax = mascara de 16 bits
    out	0A1h,al
    pop     rbp
    retn


;8254 Timer (Timer Tick)
_irq00Handler:
	pushAllState

	mov rdi, 0
	mov rsi, rsp
	call irqDispatcher

	;Usamos rsp como parámetro para scheduler en rdi
	;Como se devuelve en rax, hacemos el mov correspondiente
	mov rdi,rsp
	call scheduler
	mov rsp, rax

	;Send EOF (end of interrupt) de clase 
	mov al, 20h
	out 20h, al

	popAllState
	iretq

;Keyboard
_irq01Handler:
	irqHandlerMaster 1

;Cascade pic never called
_irq02Handler:
	irqHandlerMaster 2

;Serial Port 2 and 4
_irq03Handler:
	irqHandlerMaster 3

;Serial Port 1 and 3
_irq04Handler:
	irqHandlerMaster 4

;USB
_irq05Handler:
	irqHandlerMaster 5

;Macro para las irq de software
; %macro irqHandlerMasterSoftware 1
; 	mov rcx, rax ;movemos el contenido de rax a rcx que va a ser el codigo de syscall en caso de int80
; 	mov r8, %1  ;Pasaje de parametro
; 	mov r9, rsp ;pasaje de stackFrame
; 	call irqDispatcherSoftware

; 	iretq
; %endmacro

;software outage (int 80h)
_irq80Handler:
	mov r9, rax
	call syscall_handler

	iretq


;Zero Division Exception
_exception0Handler:
	exceptionHandler 0

;Invalid Op Code Exception
_exception6Handler:
	exceptionHandler 6

haltcpu:
	cli
	hlt
	ret


get_saved_registers:
	mov rax, regs
	ret

delete_saved_registers:
	mov qword [regs], 0
	mov qword [regs+8], 0
	mov qword [regs+(2*8)], 0
	mov qword [regs+(3*8)], 0
	mov qword [regs+(4*8)], 0
	mov qword [regs+(5*8)], 0
	mov qword [regs+(6*8)], 0
	mov qword [regs+(7*8)], 0
	mov qword [regs+(8*8)], 0
	mov qword [regs+(9*8)], 0
	mov qword [regs+(10*8)], 0
	mov qword [regs+(11*8)], 0
	mov qword [regs+(12*8)], 0
	mov qword [regs+(13*8)], 0
	mov qword [regs+(14*8)], 0
	mov qword [regs+(15*8)], 0
	ret

SECTION .bss
	aux resq 1
	regs resb 1024