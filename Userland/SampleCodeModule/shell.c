#include <shell.h>
#include <libc.h>
#include <commands.h>
#include <syscalls.h>

#define STDIN 0
#define USER 2
#define SIZE_BUF 200

static char exit;
static char buffer[SIZE_BUF]={0};
static char user[SIZE_BUF]={0};
static int userLen;

static void getBuffer();

/*Funcion que se encarga de leer lo que ingrese el usuario constantemente hasta que escriba exit*/
void shell(){
    printf("Ingrese nombre de usuario: ");
    getBuffer();
    strCpy(user,buffer);
    userLen=strLen(user);

    printf("\nBienvenido!\nComando \'/help\' para mas ayuda\n\n");
    printf("Presione 'CTRL' en cualquier momento para guardar el valor de los registros\n");
    printf("Que modulo desea correr?\n");
    getBuffer();
    exit = runCommand(checkCommand(buffer));

    while (!exit){
        putChar('$');
        sys_write(USER,user,userLen);
        printf(": ");

        getBuffer();

        exit = runCommand(checkCommand(buffer));
    }
}

/*Lee de entrada estandar lo que escribe el usuario*/
static void getBuffer(){
    int len = sys_read(STDIN, buffer, SIZE_BUF);
    buffer[len-1] = 0;
}