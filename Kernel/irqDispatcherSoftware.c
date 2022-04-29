#include <stdint.h>
#include <syscalls_management.h>
#include <irqDispatcherSoftware.h>

/*Recibe la interrupcion de numero mayor a 35 pues esta funcion maneja unicamente interrupciones que no esten en el PIC.
La misma recibe los registros como parametro y en base al rdi llama a la funcion que maneja la irq correspondiente*/
void irqDispatcherSoftware(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rax, uint64_t irq, uint64_t rsp){
	
	switch (irq) {
		case 80:
			int_80(rax, rdi, rsi, rdx, rsp);
			break;

	}
	return;
}

/*Esta funcion unicamente llama a la funcion syscall_handler() de syscalls_management (se realiza esta funcion en caso de expancion del programa)*/
void int_80(uint64_t rax, uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rsp) {
	syscall_handler(rax, rdi, rsi, rdx, rsp);
}