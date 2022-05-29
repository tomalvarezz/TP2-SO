include Makefile.inc

KERNEL=kernel.bin

SOURCES=$(wildcard *.c)
SOURCES_ASM=$(wildcard asm/*.asm)
SOURCES_DRIVERS=$(wildcard drivers/*.c)
SOURCES_INTERRUPTIONS=$(wildcard interruptions/*.c)
SOURCES_LIBRARIES=$(wildcard libraries/*.c)
SOURCES_MEMORY_MANAGER=$(wildcard memory_manager/*.c)
SOURCES_PIPES=$(wildcard pipes/*.c)
SOURCES_SCHEDULER=$(wildcard scheduler/*.c)
SOURCES_SEMAPHORES=$(wildcard semaphores/*.c)

OBJECTS=$(SOURCES:.c=.o)
OBJECTS_ASM=$(SOURCES_ASM:.asm=.o)
OBJECTS_DRIVERS=$(SOURCES_DRIVERS:.c=.o)
OBJECTS_INTERRUPTIONS=$(SOURCES_INTERRUPTIONS:.c=.o)
OBJECTS_LIBRARIES=$(SOURCES_LIBRARIES:.c=.o)
OBJECTS_MEMORY_MANAGER=$(SOURCES_MEMORY_MANAGER:.c=.o)
OBJECTS_PIPES=$(SOURCES_PIPES:.c=.o)
OBJECTS_SCHEDULER=$(SOURCES_SCHEDULER:.c=.o)
OBJECTS_SEMAPHORES=$(SOURCES_SEMAPHORES:.c=.o)

#El loader debe hacerse antes que todo pues sino la creación de kernel falla
#Por lo tanto, se hace aparte
SOURCES_LOADER=loader.asm
OBJECTS_LOADER=$(SOURCES_LOADER:.asm=.o)

MM_SELECTOR=-D KR_MEMORY_MANAGER

STATICLIBS=
ALL_OBJECTS=$(OBJECTS_LOADER) $(OBJECTS) $(OBJECTS_ASM) $(OBJECTS_DRIVERS) $(OBJECTS_INTERRUPTIONS) $(OBJECTS_INTERRUPTIONS_ASM) $(OBJECTS_LIBRARIES)  $(OBJECTS_MEMORY_MANAGER) $(OBJECTS_PIPES) $(OBJECTS_SCHEDULER) $(OBJECTS_SEMAPHORES)

all: $(KERNEL)

$(KERNEL): $(STATICLIBS) $(ALL_OBJECTS)
	$(LD) $(LDFLAGS) -T kernel.ld -o $@ $^

%.o: %.c
	$(GCC) $(GCCFLAGS) -I./include $(MM_SELECTOR) -c $< -o $@
%.o : %.asm
	$(ASM) $(ASMFLAGS) $< -o $@

$(OBJECTS_LOADER):
	$(ASM) $(ASMFLAGS) $(SOURCES_LOADER) -o $(OBJECTS_LOADER)

clean:
	rm -rf $(ALL_OBJECTS) *.bin

.PHONY: all clean