#include <keyboard.h>
#include <naiveConsole.h>
#include <syscalls_management.h>
#include <interrupts.h>
#include <scheduler.h>

#define SIZE_BUF 200

#define CAPSLOCK 0x3A
#define LSHIFT 0x2A
#define RSHIFT 0x36
#define BACKSPACE 0x0E
#define ESC 0x01
#define CTRL 0x1D
#define ESC_ASCII 27
#define EOF -1

static char buffer[SIZE_BUF] = {0};
static unsigned int index_buffer = 0;
static int mayusFlag = 0;
static char keyChar;
static int shiftFlag = 0;
static int controlFlag = 0;

// este mapa esta basado en 'Keyboard Scan Codes'
static char kbd_US[128][2] =
    {
        {0, 0},
        {0, 0},
        {'1', '!'},
        {'2', '@'},
        {'3', '#'},
        {'4', '$'},
        {'5', '$'},
        {'6', '^'},
        {'7', '&'},
        {'8', '*'},
        {'9', '('},
        {'0', ')'},
        {'-', '_'},
        {'=', '+'},
        {'\b', '\b'},
        {'\t', '\t'},
        {'q', 'Q'},
        {'w', 'W'},
        {'e', 'E'},
        {'r', 'R'},
        {'t', 'T'},
        {'y', 'Y'},
        {'u', 'U'},
        {'i', 'I'},
        {'o', 'O'},
        {'p', 'P'},
        {'[', '{'},
        {']', '}'},
        {'\n', '\n'},
        {0, 0},
        {'a', 'A'},
        {'s', 'S'},
        {'d', 'D'},
        {'f', 'F'},
        {'g', 'G'},
        {'h', 'H'},
        {'j', 'J'},
        {'k', 'K'},
        {'l', 'L'},
        {';', ':'},
        {'\'', '\"'},
        {'`', '~'},
        {0, 0},
        {'\\', '|'},
        {'z', 'Z'},
        {'x', 'X'},
        {'c', 'C'},
        {'v', 'V'},
        {'b', 'B'},
        {'n', 'N'},
        {'m', 'M'},
        {',', '<'},
        {'.', '>'},
        {'/', '?'},
        {0, 0},
        {0, 0},
        {0, 0},
        {' ', ' '},
        {0, 0}};

static int isMayus(char keyCode);
static int isShift(char keyCode);
static int isControl(char keyCode);
static int isEscape(char keyCode);

static int keyPressed(char keyCode);

/*Se invocara a esta funcion cada vez que se presione o suelte una tecla*/
void keyboard_handler() {
    char keyCode = bring_key();

    // En caso de que la tecla este presionada se ejecutara el siguiente codigo, pues si no lo controlaramos se ejecutaria todo 2 veces
    if (keyPressed(keyCode)) {

        // Si es la tecla 'CAPS LOCK' prendemos el flag y lo volvemos a apagar unicamente cuando se presione de nuevo.
        // Retornamos pues no es una tecla que debemos imprimir en pantalla
        if (isMayus(keyCode)) {
            mayusFlag = (mayusFlag == 1) ? 0 : 1;
            return;
        }

        // Si es la tecla 'RIGHT SHIFT' o 'LEFT SHIFT' prendemos el flag de la tecla.
        // Retornamos pues no es una tecla que debemos imprimir en pantalla
        if (isShift(keyCode)) {
            shiftFlag = 1;
            return;
        }

        if (isEscape(keyCode)) {
            keyChar = ESC_ASCII;
            return;
        }

        // Si la tecla es 'CTRL' guardamos los registros que se hayan guardado
        if (isControl(keyCode)) {
            // uint64_t* copy = get_saved_registers();
            // write_registers(copy);
            controlFlag = 1;
            return;
        }

        if ((mayusFlag + shiftFlag) % 2 == 0) {
            // si la suma da 0 ninguno esta apretado -> minuscula
            // si la suma da 2 los 2 estan apretados -> minuscula
            keyChar = kbd_US[keyCode][0];
        } else {
            // si la suma da 1 alguno esta apretado -> mayuscula
            keyChar = kbd_US[keyCode][1];
        }

        switch (keyChar) {
            // En caso de 'back space' borramos la letra de la pantalla y disminuimos en 1 el buffer
            case '\b':
                if (index_buffer > 0) {
                    ncPrintChar(keyChar);
                    index_buffer--;
                    buffer[index_buffer] = 0;
                }
                break;

            case 'r':
            case 'R':
                if(controlFlag){
                  uint64_t* copy = get_saved_registers();
                  write_registers(copy);
                }

            case 'd':
            case 'D':
                if(controlFlag){
                  buffer[index_buffer++] = EOF;
                }

            //Para 
            case 'c':
            case 'C':
                kill_current_FG_process();

            case 'z':
            case 'Z':

            // En caso contrario agregamos la tecla al buffer y la imprimimos en pantalla
            default:
                if(!controlFlag){
                  buffer[index_buffer++] = keyChar;
                  ncPrintChar(keyChar);
                }
                break;
        }
    }
    // Si entramos en este fragmento de codigo es porque la tecla esta subiendo
    else {
        // En caso de que la tecla este subiendo y sea cualquiera de los 2 'SHIFT' desactivamos el flag de shift
        if (keyCode == (LSHIFT - 0x80) || keyCode == (RSHIFT - 0x80)) {
            shiftFlag = 0;
        }else if (keyCode == (CTRL - 0x80)){
            controlFlag = 0;
        }
        keyChar = 0;
    }
}

// Devuelve el buffer
char* getBuffer() {
    return buffer;
}

// Resetea el buffer colocando el index en 0
void cleanBuffer() {
    index_buffer = 0;
}

// Devuelve el tamaño del buffer
int sizeBuffer() {
    return index_buffer;
}

// Devuelve la tecla que se presiono, si es que se presiono,
char kbHit() {
    return keyChar;
}

// Retorna 1 si es la tecla 'CAPSLOCK'
static int isMayus(char keyCode) {
    return keyCode == CAPSLOCK;
}

// Retorna 1 si es 'LEFT SHIFT' o si es 'RSHIFT'
static int isShift(char keyCode) {
    return keyCode == LSHIFT || keyCode == RSHIFT;
}

// Retorna 1 si es 'CTRL'
static int isControl(char keyCode) {
    return keyCode == CTRL;
}

// Retorna 1 si es 'ESC'
static int isEscape(char keyCode) {
    return keyCode == ESC;
}

// Retorna 1 si la tecla de codigo 'keycode' esta presionada
// Para saber eso basta con hacer un and con el bit mas significativo pues el mismo sera 1 si sube la tecla y 0 en caso contrario
static int keyPressed(char keyCode) {
    return (keyCode & 0b10000000) == 0b00000000;
}
