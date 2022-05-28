#include <libc.h>
#include <syscalls.h>
#include <stdint.h>
#include <commands.h>
#include <tests.h>
#include <phylo.h>

#define COM_NUM 25
#define STDIN 0
#define REGISTERS 16
#define EOF -1

static char coms[COM_NUM][25] = {"/help", "/exit", "/clear", "/time", "/inforeg",
                                 "/printmem", "/zero_division", "/invalid_op_code", "/ps", "/mem", "/loop", "/kill", "/nice", "/block",
                                 "/sem", "/cat", "/wc", "/filter", "/pipe", "/phylo", "/test_mm", "/test_processes", "/test_priority", "/test_synchro", "/test_no_synchro"};
static char coms_desc[COM_NUM][80] =
    {"Muestro ayuda",
     "Cierro la terminal",
     "Borro el contenido de la terminal",
     "Muestro en pantalla dia y hora del sistema",
     "Imprimo estado de los registros guardados",
     "Imprimo vuelco de memoria de 32 bits, tras solicitar una direccion",
     "Lanza excepcion de division por cero",
     "Lanza excepcion de codigo de operacion invalido",
     "Muestro el estado de todos los procesos",
     "Hago memory_dump del estado actual de memoria allocada",
     "Creo un proceso que imprime su id cada cierto tiempo",
     "Mato un proceso dado su ID",
     "Modifico prioridad de un proceso dado su ID y una nueva prioridad",
     "Bloqueo o pongo en ready un proceso dado su ID",
     "Muestro estado de los distintos semaforos que haya",
     "Imprimo STDIN en pantalla tal como se recibe",
     "Cuento cantidad de lineas del input",
     "Filtro vocales del input",
     "Muestro el estado de todos los pipes",
     "Corro programa de filosofos",
     "Test del memory manager",
     "Test creacion de varios procesos",
     "Test procesos con distintas prioridades",
     "Test procesos con semaforos",
     "Test procesos sin semaforos"};

static int checkCommand(char* com);
static void printHelp();
static void printTime();
static void printRegisters();
static void printMemory();
static void zeroExceptionCommand();
static void bringTime(char* finalStr);
static int checkArgc(int argc, int validNum);
static void loop();
static int isVowel(char c);
static void filter();
static void wc();
static void cat();

/*devuelve el numero de comando que se encuentra en el vector de strings 'coms' en caso de que el comando sea correcto, caso contrario retornara COM_NUM*/
static int checkCommand(char* com) {
    int i;
    for (i = 0; i < COM_NUM; i++) {
        if (strCmp(com, coms[i]) == 0) {
            return i;
        }
    }

    return i;
}

/*En base a un numero de comando pasado por parametro se ejecuta la funcion correspondiente, retorna la funcion en caso de que el comando sea exit=1*/
int runCommand(int argc, char* argv[]) {
    int exit = 0;

    int com = checkCommand(argv[0]);

    switch (com) {
        case 0:
            if (checkArgc(argc, 1) < 0) {
                break;
            }
            printHelp();
            break;

        case 1:
            if (checkArgc(argc, 1) < 0) {
                break;
            }
            exit = 1;
            printf("Salgo de userland\n");
            break;

        case 2:
            if (checkArgc(argc, 1) < 0) {
                break;
            }
            sys_clear();
            break;

        case 3:
            if (checkArgc(argc, 1) < 0) {
                break;
            }
            printTime();
            break;

        case 4:
            if (checkArgc(argc, 1) < 0) {
                break;
            }
            printRegisters();
            break;

        case 5:
            if (checkArgc(argc, 1) < 0) {
                break;
            }
            printMemory();
            break;

        case 6:
            if (checkArgc(argc, 1) < 0) {
                break;
            }
            zeroExceptionCommand();
            break;

        case 7:
            if (checkArgc(argc, 1) < 0) {
                break;
            }
            sys_invalid_op_code();
            ;
            break;

        case 8:
            if (checkArgc(argc, 1) < 0) {
                break;
            }
            sys_processes_status();
            break;

        case 9:
            if (checkArgc(argc, 1) < 0) {
                break;
            }
            sys_memory_dump();
            break;

        case 10:
            if (checkArgc(argc, 1) < 0) {
                break;
            }
            char* argv_loop[] = {"Proceso loop"};
            if (sys_new_process(&loop, 1, argv_loop, BACKGROUND, 0) < 0) {
                printf("Error al crear el loop");
            }
            break;

        case 11:
            if (checkArgc(argc, 2) < 0) {
                break;
            }
            int pid_kill = satoi(argv[1]);

            if (pid_kill < 0) {
                printf("\nArgumento invalido\n");
                break;
            }

            if (sys_kill_process(pid_kill) > 0) {
                printf("\nProceso PID: %d eliminado\n", pid_kill);
            } else {
                printf("\nNo se pudo eliminar proceso PID: %d\n", pid_kill);
            }
            break;

        case 12:
            if (checkArgc(argc, 3) < 0) {
                break;
            }
            int pid_nice = satoi(argv[1]);
            int prior = satoi(argv[2]);
            if (pid_nice < 0 || prior < 0) {
                printf("\nArgumento invalido\n");
                break;
            }

            if (sys_set_priority(pid_nice, prior) > 0) {
                printf("\nPrioridad de proceso PID: %d cambiada a %d\n", pid_nice, prior);
            } else {
                printf("\nNo se pudo cambiar prioridad de proceso PID: %d\n", pid_nice);
            }

            break;

        case 13:
            if (checkArgc(argc, 2) < 0) {
                break;
            }

            int pid_block = satoi(argv[1]);

            if (pid_block < 0) {
                printf("\nArgumento invalido\n");
                break;
            }

            int state = sys_get_process_state(pid_block);

            if (state < 0) {
                printf("\nEl proceso no existe\n");
                break;
            }

            if (state == READY) {
                printf("\nSe bloqueo proceso PID: %d\n", pid_block);
                sys_block_process(pid_block);
            }
            if (state == BLOCKED) {
                printf("\nSe desbloqueo proceso PID: %d\n", pid_block);
                sys_ready_process(pid_block);
            }

            break;

        case 14:
            if (checkArgc(argc, 1) < 0) {
                break;
            }
            sys_sem_status();
            break;

        case 15:
            if (checkArgc(argc, 1) < 0) {
                break;
            }
            cat();
            break;

        case 16:
            if (checkArgc(argc, 1) < 0) {
                break;
            }
            wc();
            break;

        case 17:
            if (checkArgc(argc, 1) < 0) {
                break;
            }
            filter();
            break;

        case 18:
            if (checkArgc(argc, 1) < 0) {
                break;
            }
            sys_pipe_status();
            break;

        case 19:
            if (checkArgc(argc, 1) < 0) {
                break;
            }

            phylosophersProblem();       
            break;

        case 20:
            if (checkArgc(argc, 1) < 0) {
                break;
            }
            char* argv_test_mm[] = {"Test Memory Manager"};
            int test_mm_PID = sys_new_process(&test_mm, 1, argv_test_mm, BACKGROUND, 0);
            if (test_mm_PID < 0) {
                printf("Error al crear el test memory manager");
            }
            printf("\nTesteo con PID: %d\n", test_mm_PID);
            break;

        case 21:
            if (checkArgc(argc, 1) < 0) {
                break;
            }
            char* argv_test_processes[] = {"Test Processes"};
            int test_processes_PID = sys_new_process(&test_mm, 1, argv_test_mm, BACKGROUND, 0);
            if (test_processes_PID < 0) {
                printf("Error al crear el test memory manager");
            }
            printf("\nTesteo con PID: %d\n", test_processes_PID);
            break;

        case 22:
            if (checkArgc(argc, 1) < 0) {
                break;
            }
            printf("test_priority\n");
            break;

        case 23:
            if (checkArgc(argc, 1) < 0) {
                break;
            }
            printf("test_synchro\n");
            break;

        case 24:
            if (checkArgc(argc, 1) < 0) {
                break;
            }
            printf("test_no_synchro\n");
            break;

        default:
            printf("Comando invalido\n");
            break;
    }
    return exit;
}

static int checkArgc(int argc, int validNum) {
    if (argc == validNum) {
        return 0;
    }
    printf("\nCantidad de parametros invalido\n");
    printf("\nCantidad de parametros esperado es: %d\n", validNum);
    return -1;
}

/*Imprime en pantalla el vector de strings de 'help'*/
static void printHelp() {
    for (int i = 0; i < COM_NUM; i++) {
        printf("%s: %s\n", coms[i], coms_desc[i]);
    }
}

/*Imprime el tiempo en pantalla*/
static void printTime() {
    char finalStr[26] = {0};
    bringTime(finalStr);
    printf("%s\n", finalStr);
}

/*Imprime los 16 registros en pantalla, utilizando la funcion en asm sys_get_register*/
static void printRegisters() {
    uint64_t registers[REGISTERS];
    if (sys_get_registers(registers)) {
        char* strRegisters[] =
            {"R15:   ", "R14:   ", "R13:   ", "R12:   ", "R11:   ", "R10:   ", "R9:    ", "R8:    ", "RSI:   ", "RDI:   ", "RBP:   ", "RDX:   ", "RCX:   ", "RBX:   ", "RAX:   ", "RSP:   "};

        for (int i = 0; i < REGISTERS; i++) {
            printf("%s%x\n", strRegisters[i], registers[i]);
        }
    } else {
        printf("Presione la tecla \'CTRL\' + \'r\' para guardar los registros\n");
    }
}

/*Le pide al usuario un numero en hexadecimal, el cual se chequea si es valido, e imprime en pantalla 32 bytes de memoria a partir de dicha direccion*/
static void printMemory() {
    int flag = 0;
    char hexaNum[20] = {0};
    uint64_t memoryDump[32]; // 32 pues el el numero de bytes que va a imprimir charde memoria

    printf("Escriba un numero en Hexadecimal: ");
    do {
        if (flag == 1) {
            printf("\nFormato incorrecto, intentelo de nuevo:");
        }
        sys_read(STDIN, hexaNum, 18);
        flag = 1;
    } while (hexaNum[0] != '0' || hexaNum[1] != 'x');

    uint64_t num = hexaStrToint(hexaNum);

    sys_get_memory(num, memoryDump);

    for (int i = 0; i < 32; i++) {
        if (i % 4 == 0) {
            printf("\n");
        }
        printf("%d: %x ", i + 1, memoryDump[i]);
    }
    printf("\n");
}

/*Fuerza una excepcion de dividir por 0*/
static void zeroExceptionCommand() {
    int x = 5 / 0;
}

/*Devuelve la fecha actual del sistema en finalStr a travez de la irq 'Timer Tick'*/
static void bringTime(char* finalStr) {
    finalStr[0] = '\0';

    uint64_t day = sys_time(0x07);
    uint64_t month = sys_time(0x08);
    uint64_t year = sys_time(0x09);

    uint64_t hours = sys_time(0x04);
    uint64_t mins = sys_time(0x02);
    uint64_t seconds = sys_time(0x00);

    char dayStr[3], monthStr[3], yearStr[3], hoursStr[3], minStr[3], secStr[3];
    if (hours != 0 && hours > 3)
        hours -= 3;
    else
        hours += 21;

    if (hours > 20)
        day -= 1;

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

    if (seconds < 10) {
        strCat(finalStr, "0");
    }
    strCat(finalStr, secStr);
}

static void loop() {
    int pid = sys_get_process_pid();
    while (1) {
        printf("\nHOLA! Soy loop %d\n", pid);
        sys_sleep(10000);
    }
}

static int isVowel(char c) {
    switch (c) {
        case 'a':
        case 'A':
        case 'e':
        case 'E':
        case 'i':
        case 'I':
        case 'o':
        case 'O':
        case 'u':
        case 'U':
            return 1;
    }
    return 0;
}

static void filter() {
    char c;
    while ((c = getChar()) != EOF) {
        if (!isVowel(c)) {
            putChar(c);
        }
    }
    putChar('\n');
}

static void wc() {
    char c;
    int lineCount = 1;
    while ((c = getChar()) != EOF) {
        if (c == '\n') {
            lineCount++;
        }
    }
    printf("\nCantidad de lineas: %d\n", lineCount);
}

static void cat() {
    int c;
    while ((c = getChar()) != EOF) {
        putChar(c);
    }
}