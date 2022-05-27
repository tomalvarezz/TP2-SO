#ifndef _LIBC_H_
#define _LIBC_H_

#include <stdarg.h>
#include <syscalls.h>

void putChar(char character);
char getChar();
void printf(char * string , ...);
int strCmp(const char* s1, const char* s2);
char* strCpy(char* dest, const char* src);
int strLen(char* s);
void swap(char *x, char *y);
char* reverse(char *buffer, int i, int j);
char* itoa(int value, char* buffer, int base);
int abs(int value);
char *strCat(char *dst, const char *src);
void intToHexa(uint64_t num, char * str, int bytes);
uint64_t hexaStrToint(char *hexaStr);
int pow(int x, unsigned int y);
void *malloc(uint64_t malloc_bytes);
void free(void *block);
int satoi(char *str);
int check_digit(char c);

#endif

