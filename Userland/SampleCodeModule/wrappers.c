#include <syscalls.h>
#include <libc.h>
#include <tests.h>
#include <phylo.h>

#define COM_NUM 25
#define REGISTERS 16
#define STDIN 0
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

void printHelpWrapper(int argc, char** argv) {
    for (int i = 0; i < COM_NUM; i++) {
        printf("%s: %s\n", coms[i], coms_desc[i]);
    }
}

int exitUserlandWrapper(int argc, char** argv) {
    printf("Salgo de userland\n");
    return 1;
}

void clearWrapper(int argc, char** argv) {
    sys_clear();
}

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

void printTimeWrapper(int argc, char** argv) {
    char finalStr[26] = {0};
    bringTime(finalStr);
    printf("%s\n", finalStr);
}

void printRegistersWrapper(int argc, char** argv) {
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

void printMemoryWrapper(int argc, char** argv) {
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

void zeroExceptionCommandWrapper(int argc, char** argv) {
    int x = 5 / 0;
}

void invalidOpCodeWrapper(int argc, char** argv) {
    sys_invalid_op_code();
}

void processesStatusWrapper(int argc, char** argv) {
    sys_processes_status();
}

void memoryDumpWrapper(int argc, char** argv) {
    sys_memory_dump();
}

void loopWrapper(int argc, char** argv) {
    int pid = sys_get_process_pid();
    while (1) {
        printf("\nHOLA! Soy loop %d\n", pid);
        sys_sleep(5000);
    }
}

void killProcessWrapper(int argc, char** argv) {
    int pid_kill = satoi(argv[1]);

    if (pid_kill < 0) {
        printf("\nArgumento invalido\n");
        return;
    }

    int kill_ret = sys_kill_process(pid_kill);

    if (kill_ret > 0) {
        printf("\nProceso PID: %d eliminado\n", pid_kill);
    } else {
        printf("\nNo se pudo eliminar proceso PID: %d\n", pid_kill);
    }
}

void niceWrapper(int argc, char** argv) {
    int pid_nice = satoi(argv[1]);
    int prior = satoi(argv[2]);
    if (pid_nice < 0 || prior < 0) {
        printf("\nArgumento invalido\n");
        return;
    }

    if (sys_set_priority(pid_nice, prior) > 0) {
        printf("\nPrioridad de proceso PID: %d cambiada a %d\n", pid_nice, prior);
    } else {
        printf("\nNo se pudo cambiar prioridad de proceso PID: %d\n", pid_nice);
    }
}

void blockWrapper(int argc, char** argv){
    int pid_block = satoi(argv[1]);

    if (pid_block < 0) {
        printf("\nArgumento invalido\n");
        return;
    }

    int state = sys_get_process_state(pid_block);

    if (state < 0) {
        printf("\nEl proceso PID %d no existe\n", pid_block);
        return;
    }

    if (state == READY) {
        printf("\nSe bloqueo proceso PID: %d\n", pid_block);
        sys_block_process(pid_block);
    }
    if (state == BLOCKED) {
        printf("\nSe desbloqueo proceso PID: %d\n", pid_block);
        sys_ready_process(pid_block);
    }
}

void semStatusWrapper(int argc, char** argv){
    sys_sem_status();
}

void catWrapper(int argc, char** argv){
    int c;
    while ((c = getChar()) != EOF) {
        putChar(c);
    }
}

void wcWrapper(int argc, char** argv){
    char c;
    int lineCount = 1;
    while ((c = getChar()) != EOF) {
        if (c == '\n') {
            lineCount++;
        }
    }
    printf("\nCantidad de lineas: %d\n", lineCount);
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

void filterWrapper(int argc, char** argv) {
    char c;
    while ((c = getChar()) != EOF) {
        if (!isVowel(c)) {
            putChar(c);
        }
    }
    putChar('\n');
}

void pipeStatusWrapper(int argc, char** argv){
    sys_pipe_status();
}

void phyloWrapper(int argc, char** argv){
    phylosophersProblem();
}

void testMmWrapper(int argc, char** argv){
    test_mm(); 
}

void testProcessesWrapper(int argc, char** argv){
    test_processes();
}

void testPriorityWrapper(int argc, char** argv){
    printf("test_priority\n");
}

void testSynchroWrapper(int argc, char** argv){
    printf("test_synchro\n");
}

void testNoSynchroWrapper(int argc, char** argv){
    printf("test_no_synchro\n");
}