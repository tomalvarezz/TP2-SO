// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <syscalls.h>
#include <stdint.h>
#include <commands.h>
#include <wrappers.h>
#include <libc.h>

#define COM_NUM 25

static char coms[COM_NUM][25] = {"/help", "/exit", "/clear", "/time", "/inforeg",
                                 "/printmem", "/zero_division", "/invalid_op_code", "/ps", "/mem", "/loop", "/kill", "/nice", "/block",
                                 "/sem", "/cat", "/wc", "/filter", "/pipe", "/phylo", "/test_mm", "/test_processes", "/test_priority", "/test_synchro", "/test_no_synchro"};

static int checkArgc(int argc, int validNum);

/*En base a un numero de comando pasado por parametro se ejecuta la funcion correspondiente, retorna la funcion en caso de que el comando sea exit=1*/
int runCommand(int argc, char* argv[], int foreground, int* fd) {
    int exit = 0;

    int com = checkCommand(argv[0]);

    switch (com) {
        case 0:
            if (checkArgc(argc, 1) < 0) {
                break;
            }
            return sys_new_process((void (*)(int, char**)) &printHelpWrapper, argc, (char**)argv, foreground, fd);

        case 1:
            if (checkArgc(argc, 1) < 0) {
                break;
            }
            break;

        case 2:
            if (checkArgc(argc, 1) < 0) {
                break;
            }
            return sys_new_process((void (*)(int, char**)) &clearWrapper, argc, (char**)argv, foreground, fd);

        case 3:
            if (checkArgc(argc, 1) < 0) {
                break;
            }
            return sys_new_process((void (*)(int, char**)) &printTimeWrapper, argc, (char**)argv, foreground, fd);

        case 4:
            if (checkArgc(argc, 1) < 0) {
                break;
            }
            return sys_new_process((void (*)(int, char**)) &printRegistersWrapper, argc, (char**)argv, foreground, fd);

        case 5:
            if (checkArgc(argc, 1) < 0) {
                break;
            }
            return sys_new_process((void (*)(int, char**)) &printMemoryWrapper, argc, (char**)argv, foreground, fd);

        case 6:
            if (checkArgc(argc, 1) < 0) {
                break;
            }
            return sys_new_process((void (*)(int, char**)) &zeroExceptionCommandWrapper, argc, (char**)argv, foreground, fd);

        case 7:
            if (checkArgc(argc, 1) < 0) {
                break;
            }
            return sys_new_process((void (*)(int, char**)) &invalidOpCodeWrapper, argc, (char**)argv, foreground, fd);

        case 8:
            if (checkArgc(argc, 1) < 0) {
                break;
            }
            return sys_new_process((void (*)(int, char**)) &processesStatusWrapper, argc, (char**)argv, foreground, fd);

        case 9:
            if (checkArgc(argc, 1) < 0) {
                break;
            }
            return sys_new_process((void (*)(int, char**)) &memoryDumpWrapper, argc, (char**)argv, foreground, fd);

        case 10:
            if (checkArgc(argc, 1) < 0) {
                break;
            }
            return sys_new_process((void (*)(int, char**)) &loopWrapper, argc, (char**)argv, foreground, fd);

        case 11:
            if (checkArgc(argc, 2) < 0) {
                break;
            }
            return sys_new_process((void (*)(int, char**)) &killProcessWrapper, argc, (char**)argv, foreground, fd);

        case 12:
            if (checkArgc(argc, 3) < 0) {
                break;
            }
            return sys_new_process((void (*)(int, char**)) &niceWrapper, argc, (char**)argv, foreground, fd);

        case 13:
            if (checkArgc(argc, 2) < 0) {
                break;
            }
            return sys_new_process((void (*)(int, char**)) &blockWrapper, argc, (char**)argv, foreground, fd);

        case 14:
            if (checkArgc(argc, 1) < 0) {
                break;
            }
            return sys_new_process((void (*)(int, char**)) &semStatusWrapper, argc, (char**)argv, foreground, fd);

        case 15:
            if (checkArgc(argc, 1) < 0) {
                break;
            }
            return sys_new_process((void (*)(int, char**)) &catWrapper, argc, (char**)argv, foreground, fd);

        case 16:
            if (checkArgc(argc, 1) < 0) {
                break;
            }
            return sys_new_process((void (*)(int, char**)) &wcWrapper, argc, (char**)argv, foreground, fd);

        case 17:
            if (checkArgc(argc, 1) < 0) {
                break;
            }
            return sys_new_process((void (*)(int, char**)) &filterWrapper, argc, (char**)argv, foreground, fd);

        case 18:
            if (checkArgc(argc, 1) < 0) {
                break;
            }
            return sys_new_process((void (*)(int, char**)) &pipeStatusWrapper, argc, (char**)argv, foreground, fd);

        case 19:
            if (checkArgc(argc, 1) < 0) {
                break;
            }
            return sys_new_process((void (*)(int, char**)) &phyloWrapper, argc, (char**)argv, foreground, fd);

        case 20:
            if (checkArgc(argc, 1) < 0) {
                break;
            }
            return sys_new_process((void (*)(int, char**)) &testMmWrapper, argc, (char**)argv, foreground, fd);

        case 21:
            if (checkArgc(argc, 1) < 0) {
                break;
            }
            return sys_new_process((void (*)(int, char**)) &testProcessesWrapper, argc, (char**)argv, foreground, fd);

        case 22:
            if (checkArgc(argc, 1) < 0) {
                break;
            }
            return sys_new_process((void (*)(int, char**)) &testPriorityWrapper, argc, (char**)argv, foreground, fd);

        case 23:
            if (checkArgc(argc, 1) < 0) {
                break;
            }
            return sys_new_process((void (*)(int, char**)) &testSynchroWrapper, argc, (char**)argv, foreground, fd);

        case 24:
            if (checkArgc(argc, 1) < 0) {
                break;
            }
            return sys_new_process((void (*)(int, char**)) &testNoSynchroWrapper, argc, (char**)argv, foreground, fd);

        default:
            printf("Comando invalido COMMANDS\n");
            break;
    }
    return -1;
}

/*devuelve el numero de comando que se encuentra en el vector de strings 'coms' en caso de que el comando sea correcto, caso contrario retornara COM_NUM*/
int checkCommand(char* com) {
    int i;
    for (i = 0; i < COM_NUM; i++) {
        if (strCmp(com, coms[i]) == 0) {
            return i;
        }
    }
    return -1;
}

static int checkArgc(int argc, int validNum) {
    if (argc == validNum) {
        return 0;
    }
    printf("\nCantidad de parametros invalido\n");
    printf("\nCantidad de parametros esperado es: %d\n", validNum);
    return -1;
}