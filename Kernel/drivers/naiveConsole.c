#include <naiveConsole.h>
#include <syscalls_management.h>

#define DEFAULT_FONT_COLOR 0x0F

static uint32_t uintToBase(uint64_t value, char * buffer, uint32_t base);
static void ncScroll();

static char buffer[64] = { '0' };
static uint8_t * const video = (uint8_t*)0xB8000;
static uint8_t * currentVideo = (uint8_t*)0xB8000;
static const uint32_t width = 80;
static const uint32_t height = 25;

void ncPrint(const char * string)
{
	ncPrintColor(string, DEFAULT_FONT_COLOR);
}

void ncPrintColor(const char * string, char color)
{
	int i;

	for (i = 0; string[i] != 0; i++){
		ncPrintCharColor(string[i], color);
	}
}

void ncPrintCharColor(char character, char color)
{
	if(currentVideo >= video+height*width*2){
		ncScroll();
	}

	if(character=='\n'){
		ncNewline();
	}
	else if(character=='\t'){
        ncPrint("     ");
	}
	else if(character=='\b'){
		ncDeleteChar(*currentVideo);
	}
	else{
		*currentVideo = character;
		*(currentVideo+1) = color;
		currentVideo += 2;
	}
}

void ncPrintChar(char character)
{
	ncPrintCharColor(character, DEFAULT_FONT_COLOR);
}

//Funcion utilizada para implementar el backspace a la hora de escribir
void ncDeleteChar(char character){
	if(currentVideo > video ){
		if(character == '\t'){
			for(int i = 0 ; i < 5 ;i++){
				ncDeleteChar(' ');
			}
		}else if(character == '\n'){
			do
			{
				ncDeleteChar(' ');
			}while((uint64_t)(currentVideo - video) % (width * 2) != 0 && (*(currentVideo-2)==' '));
		}else {
			currentVideo-=2;
			*currentVideo = ' ';
		}
	}
}

void ncNewline()
{
	do
	{
		ncPrintChar(' ');
	}
	while((uint64_t)(currentVideo - video) % (width * 2) != 0);
}

void ncPrintDec(uint64_t value)
{
	ncPrintBase(value, 10);
}

void ncPrintHex(uint64_t value)
{
	ncPrintBase(value, 16);
}

void ncPrintBin(uint64_t value)
{
	ncPrintBase(value, 2);
}

void ncPrintBase(uint64_t value, uint32_t base)
{
    uintToBase(value, buffer, base);
    ncPrint(buffer);
}

//Funcion para limpiar la pantalla
void ncClear()
{
	int i;

	for (i = 0; i < height * width; i++)
		video[i * 2] = ' ';
	currentVideo = video;
}

static uint32_t uintToBase(uint64_t value, char * buffer, uint32_t base)
{
	char *p = buffer;
	char *p1, *p2;
	uint32_t digits = 0;

	//Calculate characters for each digit
	do
	{
		uint32_t remainder = value % base;
		*p++ = (remainder < 10) ? remainder + '0' : remainder + 'A' - 10;
		digits++;
	}
	while (value /= base);

	// Terminate string in buffer.
	*p = 0;

	//Reverse string in buffer.
	p1 = buffer;
	p2 = p - 1;
	while (p1 < p2)
	{
		char tmp = *p1;
		*p1 = *p2;
		*p2 = tmp;
		p1++;
		p2--;
	}

	return digits;
}

//Funcion para mover las lineas hacia arriba en caso de que la shell se encuentre llena
static void ncScroll(){
    int i;
	for (i = 0; i < height * width; i++){
		video[(i*2)] = video[(i+width)*2];
		video[(i*2)+1] = video[(i+width)*2 + 1];
	}

	currentVideo = video + width*(height-1)*2;
}