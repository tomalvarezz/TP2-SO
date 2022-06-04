// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
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
#define HEAP_SIZE (1024 * 1024 * 64) // 64MB

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

int main() {
    ncClear();
    initialize_memory_manager((char*)sampleCodeModuleHeapAddress, HEAP_SIZE);
    initialize_scheduler();

    backUpRipRsp((uint64_t*)sampleCodeModuleAddress, getRSP());

    char* argv[] = {"Userland Init"};
    new_process(sampleCodeModuleAddress, 1, argv, FOREGROUND, 0);

    load_idt();

    return 0;
}