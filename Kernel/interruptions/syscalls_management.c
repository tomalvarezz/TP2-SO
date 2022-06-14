// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <syscalls_management.h>
#include <naiveConsole.h>
#include <exceptions.h>
#include <libraryc.h>
#include <scheduler.h>
#include <semaphores.h>
#include <pipes.h>

#include <shared_memory.h>

#define STDIN 0
#define STDOUT 1
#define USER 2
#define REGISTERS 16
#define EOF -1

static uint64_t registers[REGISTERS];
static int canPrintRegisters = 0;

/*En base a el codigo de syscall, que se encuentra en rax, se llama a la funcion del handler correspondiente*/
uint64_t syscall_handler(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t rax) {
    switch (rax) {
        case 0:
            return sys_read_handler(rdi, (char*)rsi, rdx);

        case 1:
            return sys_write_handler(rdi, (const char*)rsi, rdx);

        case 2:
            return sys_time_handler(rdi);

        case 3:
            ncClear();
            break;

        case 4:
            return sys_get_registers_handler((uint64_t*)rdi);

        case 5:
            sys_get_memory_handler((uint8_t*)rdi, (uint64_t*)rsi);
            break;

        // 6,7,8 memory_manager syscalls
        case 6:
            return malloc((uint64_t)rdi);

        case 7:
            free((void*)rdi);
            break;

        case 8:
            memory_dump();
            break;

        case 9:
            return sys_sleep_handler(rdi);
  
        //10, 18 scheduler syscalls
        case 10:
            return new_process((void (*)(int, char**))rdi, (int)rsi, (char**)rdx, (int)rcx, (int*)r8);

        case 11:
            return kill_process((uint64_t) rdi);

        case 12:
            return block_process((uint64_t) rdi);

        case 13:
            return ready_process((uint64_t) rdi);

        case 14:
            return get_process_PID();
        
        case 15:
            return set_priority((uint64_t) rdi, (int) rsi);

        case 16: 
            print_processes_status();
            break;
        
        case 17:
            print_current_process();
            break;

        case 18:
            return get_process_state((uint64_t) rdi);

        case 19:
            yield();
            break;

        //20,24 semaphore syscalls
        case 20:
            return sem_open((uint32_t)rdi, (uint64_t) rsi);

        case 21:
            return sem_wait((uint32_t)rdi);
        
        case 22:
            return sem_post((uint32_t) rdi);
        
        case 23:
            return sem_close((uint32_t) rdi);
        
        case 24:
            sem_status();
            break;

        //25, 29pipes syscalls
        case 25:
            return pipe_open((uint64_t) rdi);
        
        case 26:
            return pipe_write((uint64_t) rdi, (char*)rsi, (uint64_t) rdx);
        
        case 27:
            return pipe_read((uint64_t) rdi);

        case 28:
            return pipe_close((uint64_t) rdi);

        case 29:
            pipe_status((uint64_t) rdi);
            break;

        case 30:
            return shm_open((int) rdi);

        default:
            break;
    }

    return 0;
}

uint64_t sys_read_handler(uint64_t fd, char* buf, uint64_t count) {
    int i=0;
    fd=get_current_process_read_FD();
    if(fd==STDIN){
        _sti();
        char* inBuffer;
        int sizebuff;

        // colocamos en *inBuffer lo que el ingresa el usuario hasta el '/n'
        do {
            inBuffer = getBuffer();
            // con esta instruccion logramos que no este pidiendo constantemente una tecla, sino que espera a la proxima interrupcion
            _hlt();
        } while ((sizebuff = sizeBuffer()) < count && inBuffer[sizebuff - 1] != '\n');

        _cli();
        
        // colocamos en buf lo que el usuario volco en el buffer siempre y cuando el file descriptor sea entrada estandar
        for (i = 0; i < count && i < sizebuff; i++) {
            buf[i] = inBuffer[i];
        }
        cleanBuffer();
    }
    else{
        char c;
        while(i<count){
            c=pipe_read(fd);
            buf[i++]=c;
        }
    }
    
    return i;
}

uint64_t sys_write_handler(uint64_t fd, const char* buf, uint64_t count) {
    fd=get_current_process_write_FD();

    // en caso de que el file descriptior sea salida estandar imprimimos en pantalla lo que se encuentre en buf
    if (fd == STDOUT) {
        for (int i = 0; i < count; i++)
            ncPrintChar(buf[i]);
        // en caso de que el file descriptor sea USER(define hecho en el codigo) imprimimos en pantalla lo que se encuentre en buff de color verde y fondo negro
    } else if (fd == USER) {
        for (int i = 0; i < count; i++)
            ncPrintCharColor(buf[i], 0x02);
    }
    else {
        pipe_write(fd, buf, count);
    }

    return count;
}

/*Llama a la funcion de asm RTC que recibe un codigo segun que unidad de tiempo se desee y devuelve el mismo
Para mas informacion visitar el sitio https://stanislavs.org/helppc/cmos_ram.html*/
uint64_t sys_time_handler(uint64_t time) {
    return RTC(time);
}

/*Recibe un vector con los 16 registros y los deposita en el vector dest*/
int sys_get_registers_handler(uint64_t* dest) {
    if (canPrintRegisters) {
        for (int i = 0; i < REGISTERS; i++) {
            dest[i] = registers[i];
        }
    }
    return canPrintRegisters;
}

/*Funcion que guarda los registros guardados */
void write_registers(uint64_t* src) {
    canPrintRegisters = 1;
    for (int i = 0; i < REGISTERS; i++) {
        registers[i] = src[i];
    }
}

/*Recibe un puntero que es la direccion de comienzo desde la cual se volcara la cantidad de memoria correspondiente en dump */
void sys_get_memory_handler(uint8_t* start, uint64_t* dump) {
    for (int i = 0; i < 32; i++) {
        dump[i] = start[i];
    }
}

/*duerme el sistema durante n mili segundos*/
uint64_t sys_sleep_handler(uint64_t timeout_ms) {
    uint64_t start_ms = millis();
    do {
        _hlt();
    } while (millis() - start_ms < timeout_ms);
    return 1;
}