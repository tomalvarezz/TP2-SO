// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <time.h>
#include <keyboard.h>
#include <stdint.h>

/*Luego de una interrupcion entre la 20 y 35, pues son las que se encuentran en el PIC.
En base a dicho codigo llama a la funcion correspondiente*/
void irqDispatcher(uint64_t irq, uint64_t rsp) {
	switch (irq) {
		case 0:
			int_20();
			break;
		case 1:
			int_21();
			break;
	}
	return;
}

/*llama a la funcion timer_handler de keyboard.c, la cual maneja la interrupcion correspondiente*/
void int_20() {
	timer_handler();
}

/*llama a la funcion keyboard_handler de keyboard.c, la cual maneja la interrupcion correspondiente*/
void int_21() {
	keyboard_handler();
}
