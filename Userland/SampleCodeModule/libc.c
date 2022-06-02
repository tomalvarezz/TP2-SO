#include <libc.h>

#define STDOUT 1
#define STDIN 0
#define FMT_DECIMAL 10

/*Escribie en pantalla el caracter enviado por parametro*/
void putChar(char character) {
    char str[1] = {character};
    sys_write(STDOUT, str, 1);
}

/*Retorna un caracter pedido al usuario en entrada estandar*/
char getChar() {
    char character[1];
    sys_read(STDIN, character, 1);
    return character[0];
}

/*Imprime en pantalla el string ingresado.
Tiene la capacidad de recibir formatos en el mismo, para mas informacion de la implementacion ingresar a: https://www.tutorialspoint.com/c_standard_library/stdarg_h.htm */
void printf(char* string, ...) {
    int i = 0, indexBuffer = 0;
    char buff[100] = {0}, aux[20];
    char* aux_str;
    va_list ap;
    va_start(ap, string);

    while (string[i]) {
        if (string[i] == '%') {
            i++;
            switch (string[i]) {
                case 'd':
                    itoa(va_arg(ap, int), aux, FMT_DECIMAL);
                    for (int i = 0; aux[i] != 0; i++) {
                        buff[indexBuffer++] = aux[i];
                    }
                    break;

                case 'c':
                    buff[indexBuffer++] = (char)va_arg(ap, int);
                    break;

                case 's':
                    aux_str = (char*)va_arg(ap, char*);
                    for (int j = 0; aux_str[j] != 0; j++) {
                        buff[indexBuffer++] = aux_str[j];
                    }
                    break;

                case 'x':
                    intToHexa(va_arg(ap, int), aux, 4);
                    for (int i = 0; aux[i] != 0; i++) {
                        buff[indexBuffer++] = aux[i];
                    }
                    break;

                default:
                    // tiramos excepcion por formato invalido
                    break;
            }

        } else {
            buff[indexBuffer++] = string[i];
        }
        i++;
    }
    sys_write(STDOUT, buff, indexBuffer);
    va_end(ap);
}

/*Devuelve 1 en caso de que los dos strings sean iguales y 0 en caso contrario*/
int strCmp(const char* s1, const char* s2) {
    while (*s1 && *s2 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

/*Copia el string src en dest*/
char* strCpy(char* dest, const char* src) {
    int i;
    for (i = 0; src[i]; i++) {
        dest[i] = src[i];
    }
    dest[i] = 0;
    return dest;
}

/*Devuelve la longitud del string s*/
int strLen(char* s) {
    int i = 0;
    while (s[i++])
        ;
    return i - 1;
}

/*A partir de aca el siguiente codigo fue copiado de: https://www.techiedelight.com/implement-itoa-function-in-c/ */
// Function to swap two numbers
void swap(char* x, char* y) {
    char t = *x;
    *x = *y;
    *y = t;
}

// Function to reverse `buffer[i…j]`
char* reverse(char* buffer, int i, int j) {
    while (i < j) {
        swap(&buffer[i++], &buffer[j--]);
    }

    return buffer;
}

int abs(int value) {
    return (value < 0) ? -1 * value : value;
}

// Iterative function to implement `itoa()` function in C
char* itoa(int value, char* buffer, int base) {
    // invalid input
    if (base < 2 || base > 32) {
        return buffer;
    }

    // consider the absolute value of the number
    int n = abs(value);

    int i = 0;
    while (n) {
        int r = n % base;

        if (r >= 10) {
            buffer[i++] = 65 + (r - 10);
        } else {
            buffer[i++] = 48 + r;
        }

        n = n / base;
    }

    // if the number is 0
    if (i == 0) {
        buffer[i++] = '0';
    }

    // If the base is 10 and the value is negative, the resulting string
    // is preceded with a minus sign (-)
    // With any other base, value is always considered unsigned
    if (value < 0 && base == 10) {
        buffer[i++] = '-';
    }

    buffer[i] = '\0'; // null terminate string

    // reverse the string and return it
    return reverse(buffer, 0, i - 1);
}

// Function to implement `strcat()` function in C
char* strCat(char* destination, const char* source) {
    int i, j;

    // move to the end of the destination string
    for (i = 0; destination[i] != '\0'; i++)
        ;

    // `i` now point to terminating null character in the destination

    // Appends characters of the source to the destination string
    for (j = 0; source[j] != '\0'; j++) {
        destination[i + j] = source[j];
    }

    // null terminate destination string
    destination[i + j] = '\0';

    // the destination is returned by standard `strcat()`
    return destination;
}

/*Fragmento de codigo copiado de: https://www.geeksforgeeks.org/program-decimal-hexadecimal-conversion/ */
void intToHexa(uint64_t num, char* str, int bytes) {
    // counter for hexadecimal number array
    int i = 0;
    while (i < bytes * 2 && num != 0) {

        // storing remainder in temp variable.
        int temp = num % 16;

        // check if temp < 10
        if (temp < 10)
            str[i++] = temp + '0';
        else
            str[i++] = temp - 10 + 'A';

        num = num / 16;
    }

    while (i < bytes * 2)
        str[i++] = '0';

    str[i++] = 'x';
    str[i++] = '0';
    str[i] = '\0';

    // Reverse the string
    reverse(str, 0, i - 1);
}

uint64_t hexaStrToint(char* hexaStr) {
    int len = strLen(hexaStr);
    uint64_t ans = 0;
    int j = 0;

    for (int i = len - 2; i >= 2; i--, j++) {
        if (hexaStr[i] <= '9' && hexaStr[i] >= '0') {
            ans += ((hexaStr[i] - '0') * pow(16, j));
        } else if ((hexaStr[i] <= 'F' && hexaStr[i] >= 'A')) {
            ans += ((hexaStr[i] - 'A' + 10) * pow(16, j));
        } else if ((hexaStr[i] <= 'f' && hexaStr[i] >= 'a')) {
            ans += ((hexaStr[i] - 'a' + 10) * pow(16, j));
        }
    }

    return ans;
}

int pow(int x, unsigned int y) {
    int result = x;
    for (int i = 1; i < y; i++) {
        result = result * x;
    }
    return result;
}

void* malloc(uint64_t size) {
    return (void*)sys_malloc(size);
}

void free(void* address) {
    sys_free(address);
}

// AGODIO
int satoi(char* str) {
    // Initialize result
    int res = 0;

    // Initialize index of first digit
    if (check_digit(str) == 0) {
        printf("Parametro invalido, PID debe ser un numero\n");
        return -1;
    }

    // Iterate through all digits
    // and update the result
    for (int i = 0; str[i] != '\0'; ++i)
        res = res * 10 + str[i] - '0';

    // Return result with sign
    return res;
}

int check_digit(char* str) {
    int flag = 1;
    for (int i = 0; str[i] && flag; i++) {
        if (str[i] < '0' || str[i] > '9') {
            flag = 0;
        }
    }
    return flag;
}