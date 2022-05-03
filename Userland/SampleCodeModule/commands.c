#include <libc.h>
#include <syscalls.h>
#include <stdint.h>
#include <commands.h>

#define ESC 27
#define COM_NUM 9
#define STDIN 0
#define REGISTERS 16

static char coms[COM_NUM][25]={"help","exit","clear","time","inforeg","printmem","split","zero_division","invalid_op_code"};
static char coms_desc[COM_NUM][80]=
{ "Muestro ayuda", 
"Cierro la terminal", 
"Borro el contenido de la terminal",
"Muestro en pantalla dia y hora del sistema", 
"Imprimo estado actual de los registros",
"Imprimo vuelco de memoria de 32 bits, tras solicitar una direccion",
"Divido pantalla en 4. (DEPRECATED)",
"Lanza excepcion de division por cero",
"Lanza excepcion de codigo de operacion invalido"};

static void printHelp();
static void printTime();
static void printRegisters();
static void printMemory();
static void zeroExceptionCommand();
static void invalidOpCodeExceptionCommand();
static void bringTime(char *finalStr);
static int isAlpha(char letter);
static int isNum(char letter);

/*devuelve el numero de comando que se encuentra en el vector de strings 'coms' en caso de que el comando sea correcto, caso contrario retornara COM_NUM*/
int checkCommand(char* com){
    int i;
    for( i=0; i < COM_NUM  ;i++){
        if(strCmp(com,coms[i])==0){
            return i;
        }
    }
    return i;
}

/*En base a un numero de comando pasado por parametro se ejecuta la funcion correspondiente, retorna la funcion en caso de que el comando sea exit=1*/
int runCommand(int com){
    int exit = 0;

    switch (com)
    {
    case 0:
        printHelp();
        break;

    case 1:
        exit=1;
        printf("Salgo de userland\n");
        break;
    
    case 2:
        sys_clear();
        break;

    case 3:
        printTime();
        break;
    
    case 4:
        printRegisters();
        break;

    case 5:
        printMemory();
        break;
    
    case 6:
        break;

    case 7:
        zeroExceptionCommand();
        break;

    case 8:
        invalidOpCodeExceptionCommand();
        break;

    default:
        printf("Comando invalido\n");
        break;
    }

    return exit;
}

/*Imprime en pantalla el vector de strings de 'help'*/
static void printHelp(){
    for (int i = 0; i < COM_NUM; i++)
    {
        printf("%s: %s\n",coms[i],coms_desc[i]);
    }
}

/*Imprime el tiempo en pantalla*/
static void printTime(){
    char finalStr[26] = {0};
    bringTime(finalStr);
    printf("%s\n", finalStr);
}

/*Imprime los 16 registros en pantalla, utilizando la funcion en asm sys_get_register*/
static void printRegisters(){
    uint64_t registers[REGISTERS];
    if(sys_get_registers(registers)){
        char *strRegisters[] =     
    {"R15:   ", "R14:   ", "R13:   ", "R12:   ", "R11:   ", "R10:   ", "R9:    ", "R8:    ", "RSI:   ", "RDI:   ", "RBP:   ", "RDX:   ", "RCX:   ", "RBX:   ", "RAX:   ", "RSP:   "};

        for (int i = 0; i < REGISTERS; i++){
            printf("%s%x\n",strRegisters[i],registers[i]);
        }
    }
    else{
        printf("Presione la tecla \'CTRL\' para guardar los registros\n");
    }
}

/*Le pide al usuario un numero en hexadecimal, el cual se chequea si es valido, e imprime en pantalla 32 bytes de memoria a partir de dicha direccion*/ 
static void printMemory(){
    int flag=0;
    char hexaNum[20]={0};
    uint64_t memoryDump[32]; //32 pues el el numero de bytes que va a imprimir charde memoria

    printf("Escriba un numero en Hexadecimal: ");
    do{
        if(flag == 1){
            printf("\nFormato incorrecto, intentelo de nuevo: ");
        }
        sys_read(STDIN, hexaNum, 18);
        flag = 1;
    }while(hexaNum[0]!='0' || hexaNum[1]!='x');

    uint64_t num=hexaStrToint(hexaNum);

    sys_get_memory(num,memoryDump);

    for (int i = 0; i < 32; i++){
        if(i%4==0){
            printf("\n");
        }
        printf("%d: %x ",i+1,memoryDump[i]);
    }
    printf("\n");
}

/*Fuerza una excepcion de dividir por 0*/
static void zeroExceptionCommand(){
    int x=5/0;
}

/*Fuerza un un llamado a la syscall por codigo invalido*/
static void invalidOpCodeExceptionCommand(){
    sys_invalid_op_code();
}

/*Devuelve la fecha actual del sistema en finalStr a travez de la irq 'Timer Tick'*/
static void bringTime(char *finalStr) {
    finalStr[0] = '\0';

    uint64_t day = sys_time(0x07);
    uint64_t month = sys_time(0x08);
    uint64_t year = sys_time(0x09);

    uint64_t hours = sys_time(0x04);
    uint64_t mins = sys_time(0x02);
    uint64_t seconds = sys_time(0x00);

    char dayStr[3], monthStr[3], yearStr[3], hoursStr[3], minStr[3], secStr[3];
    if(hours!=0 && hours>3 )
        hours -=3;
    else
        hours +=21;

    if(hours>20)
        day-=1;

    itoa(hours, hoursStr, 10);
    itoa(mins, minStr, 10);
    itoa(seconds, secStr, 10);
    itoa(day, dayStr, 10);
    itoa(month, monthStr, 10);
    itoa(year, yearStr, 10);

    strCat(finalStr, dayStr);
    strCat(finalStr, "/");
    strCat(finalStr, monthStr);
    strCat(finalStr, "/");
    strCat(finalStr, yearStr);
    strCat(finalStr, "\t");
    strCat(finalStr, hoursStr);
    strCat(finalStr, ":");
    strCat(finalStr, minStr);
    strCat(finalStr, ":");

    if(seconds<10){
        strCat(finalStr,"0");
    }
    strCat(finalStr, secStr);
}

/*Devuelve 1 si el caracter es una letra y 0 en caso contrario*/
static int isAlpha(char letter){
     return (letter>='a' && letter<='z') || (letter>='A' && letter<='Z'); 
 }

/*Devuelve 1 si el caracter es un numero y 0 en caso contrario*/
static int isNum(char letter){
    return (letter>='1' && letter<='9'); 
}