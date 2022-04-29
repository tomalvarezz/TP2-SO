#include <stdint.h>
#include <exceptions.h>
#include <naiveConsole.h>
#include <syscalls_management.h>

#define ZERO_EXCEPTION_ID 0
#define INVALID_OP_CODE_ID 6

static void zero_division();
static void invalid_opcode();
static void swap(char *x, char *y);
static char* reverse(char *buffer, int i, int j);
static void intToHexa(uint64_t num, char * str, int bytes);

static uint64_t *ripReturn;
static uint64_t *rspReturn;

/*En base al codigo de excepcion llama a la funcion que maneja la misma, luego imprime todos los registros.
Finalmente despues de 10 segundos resetea la terminal*/
void exceptionDispatcher(uint64_t exception, uint64_t *stackFrame) {
	ncClear();
	switch (exception)
	{
	case ZERO_EXCEPTION_ID:
		zero_division();
		break;
	case INVALID_OP_CODE_ID:
		invalid_opcode();
		break;
	default:
		break;
	}

    char *strRegisters[] =     
    {"R15:   ", "R14:   ", "R13:   ", "R12:   ", "R11:   ", "R10:   ", "R9:    ", "R8:    ", "RSI:   ", "RDI:   ", "RBP:   ", "RDX:   ", "RCX:   ", "RBX:   ", "RAX:   ", "RIP:   ", "CS:    ", "FLAGS: ", "RSP:   "};
	char aux[20];

    ncPrint("Valores de los registros al momento de la excepcion:\n");

    for (int i = 0; i < 19; i++){
        ncPrintColor(strRegisters[i],0x03);
		intToHexa(stackFrame[i],aux,8);
		ncPrintColor(aux,0x09);
		ncPrintChar('\n');
    }
	
	stackFrame[0xF] = (uint64_t)ripReturn;
	stackFrame[0x12] = (uint64_t)rspReturn;

	ncPrint("\nLa terminal se reiniciara en 10 segs");
	sys_sleep_handler(10000);
	
	ncClear();
}

/*Arroja el mensaje de excepcion a salida estandar de division por 0*/
static void zero_division() {
	ncPrintColor("Exception 0: Zero division exception\n",0x04);
}

/*Arroja el mensaje de excepcion a salida estandar de codigo invalido*/
static void invalid_opcode(){
	ncPrintColor("Exception 6: Invalid operation code exception\n",0x04);
}

void backUpRipRsp(uint64_t *rip, uint64_t *rsp){
	ripReturn = rip;
	rspReturn = rsp;
}


//Codigo sacadop de: https://www.geeksforgeeks.org/program-decimal-hexadecimal-conversion/
static void intToHexa(uint64_t num, char * str, int bytes){ 
    // counter for hexadecimal number array
    int i = 0;
    while (i < bytes*2 && num != 0) {
        // storing remainder in temp variable.
        int temp = num % 16;
 
        // check if temp < 10
        if (temp < 10)
            str[i++] = temp + '0';
        else
            str[i++] = temp - 10 + 'A';
 
        num = num / 16;
    }

    while (i < bytes*2)
        str[i++] = '0';

    str[i++] = 'x';
    str[i++] = '0';
    str[i] = '\0';

    // Reverse the string 
    reverse(str, 0,i-1); 
}

// Function to reverse `buffer[i…j]`
static char* reverse(char *buffer, int i, int j){
    while (i < j) {
        swap(&buffer[i++], &buffer[j--]);
    }
 
    return buffer;
}

// Function to swap two numbers
static void swap(char *x, char *y){
    char t = *x; *x = *y; *y = t;
}