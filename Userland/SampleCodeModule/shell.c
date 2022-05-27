#include <shell.h>
#include <libc.h>
#include <commands.h>
#include <syscalls.h>

#define STDIN 0
#define USER 2
#define SIZE_BUF 100
#define MAX_ARGUMENTS 7

static char exit;

static char user[SIZE_BUF]={0};
static int userLen;

static int getBuffer(char*, char** );

/*Funcion que se encarga de leer lo que ingrese el usuario constantemente hasta que escriba exit*/
void shell(){
    printf("Ingrese nombre de usuario: ");
    userLen= sys_read(STDIN, user, SIZE_BUF);
    user[--userLen]=0;

    printf("\nBienvenido!\nComando \'/help\' para mas ayuda\n\n");
    printf("Presione 'CTRL' en cualquier momento para guardar el valor de los registros\n");
    printf("Que modulo desea correr?\n");

    int argc;
    char *argv[MAX_ARGUMENTS] = {0};
    int buff_size;
    char buffer[SIZE_BUF]={0};

    //getBuffer();
    //exit = runCommand(checkCommand(buffer));

    while (!exit){
        putChar('$');
        sys_write(USER,user,userLen);
        printf(": ");

        buff_size=sys_read(STDIN, buffer, SIZE_BUF);
        buffer[buff_size-1]=0;

        argc = getBuffer(buffer,argv);

        exit = runCommand(checkCommand(argv[0]));
    }
}

/*Lee de entrada estandar lo que escribe el usuario*/
static int getBuffer(char* buffer, char** argv){

    int argc = 0;

    if (*buffer != ' ' && *buffer != '\0') {
        argv[argc++] = buffer;
    }

    while (*buffer != 0) {
        if (*buffer == ' ') {
        *buffer = 0;
            if ((*(buffer + 1) != ' ') && (*(buffer + 1) != 0)) {
                if (argc >= MAX_ARGUMENTS) {
                    return -1;
                }
                argv[argc++] = buffer + 1;
            }
        }
        buffer++;
    }
    return argc;
}