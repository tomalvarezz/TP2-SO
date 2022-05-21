#include <stdint.h>
#include <lib.h>
#include <moduleLoader.h>
#include <naiveConsole.h>
#include <idtLoader.h>
#include <exceptions.h>
#include <memory_manager.h>
#include <scheduler.h>
#include <libraryc.h>

#define STDIN 0
#define STDOUT 1
#define STDERR 2
#define HEAP_SIZE 1024 * 1024 * 64 // 64MB

extern uint8_t text;
extern uint8_t rodata;
extern uint8_t data;
extern uint8_t bss;
extern uint8_t endOfKernelBinary;
extern uint8_t endOfKernel;

static const uint64_t PageSize = 0x1000;

static void* const sampleCodeModuleAddress = (void*)0x400000;
static void* const sampleDataModuleAddress = (void*)0x500000;
static void* const sampleCodeModuleHeapAddress = (void*)0x600000;

typedef int (*EntryPoint)();

static int to_finish = 0;
static int to_block = 0;
static int blocked_PID=0;

void clearBSS(void* bssAddress, uint64_t bssSize) {
    memset(bssAddress, 0, bssSize);
}

void* getStackBase() {
    return (void*)((uint64_t)&endOfKernel + PageSize * 8 // The size of the stack itself, 32KiB
                   - sizeof(uint64_t)                    // Begin at the top of the stack
    );
}

void* initializeKernelBinary() {
    void* moduleAddresses[] = {
        sampleCodeModuleAddress,
        sampleDataModuleAddress};

    loadModules(&endOfKernelBinary, moduleAddresses);
    clearBSS(&bss, &endOfKernel - &bss);
    return getStackBase();
}

void suma(int argc, char* argv[]) {
    while (1) {
        sys_sleep_handler(2000);
        printf("Estoy en funcion suma\n");
		to_block++;
		if(to_block==10){
			ready_process(blocked_PID);
			printf("Desbloqueo proceso resta\n");
		}
    }
}

void resta(int argc, char* argv[]) {
    while (1) {
		if(to_block==6){
			printf("Bloqueo proceso resta\n");
			blocked_PID=get_process_PID();
			printf("PID %d\n", blocked_PID);
			block_process(blocked_PID);
		}
		
        sys_sleep_handler(2000);
        printf("Estoy en funcion resta\n");
		to_block++;
    }
}

void mult(int argc, char* argv[]) {
    while (1) {
        if (to_finish == 5) {
            printf("Mato proceso mult\n");
            kill_process(get_process_PID());
        }
        sys_sleep_handler(2000);
        printf("Estoy en funcion mult %s\n", argv[1]);
        to_finish++;
    }
}

int main() {

    ncClear();
    printf("Entrando a kernel");
    initialize_memory_manager((char*)sampleCodeModuleHeapAddress, HEAP_SIZE);
    printf("Inicializado memory_manager");

    initialize_scheduler();
    printf("Inicializado scheduler");

    // cargamos la tabla idt nos guardamos la direccion del inicio del programa
    //  en caso de que la necesitemos y luego llamamos al USERLAND
    char* argvsuma[] = {"suma", "1", "2"};
    new_process(&suma, 1, argvsuma, FOREGROUND, 0);

    char* argvresta[] = {"resta", "2", "2"};
    new_process(&resta, 1, argvresta, FOREGROUND, 0);

    char* argvmult[] = {"mult", "2", "2"};
    new_process(&mult, 1, argvmult, FOREGROUND, 0);

    load_idt();
    backUpRipRsp((uint64_t*)sampleCodeModuleAddress, getRSP());
    ncClear();
    ((EntryPoint)sampleCodeModuleAddress)();

    return 0;
}