include Makefile.inc

KERNEL=kernel.bin

ifeq ($(filter buddy,$(MAKECMDGOALS)),buddy)
    BUDDY=YES
else
    BUDDY=NO
endif

ifeq ($(BUDDY), YES)
SOURCES_ALL=$(wildcard *.c drivers/*.c interrupt/*.c scheduler/*.c memoryManager/buddy/*.c)
else
SOURCES_ALL=$(wildcard *.c drivers/*.c interrupt/*.c scheduler/*.c memoryManager/standard/*.c)
endif

SOURCES=$(SOURCES_ALL)
SOURCES_ASM=$(wildcard asm/*.asm)
OBJECTS=$(SOURCES:.c=.o)
OBJECTS_ASM=$(SOURCES_ASM:.asm=.o)
LOADERSRC=loader.asm

LOADEROBJECT=$(LOADERSRC:.asm=.o)
STATICLIBS=

STACK_SIZE ?= 4096

all: STACK_SIZE=4096
buddy: STACK_SIZE=25000

$(KERNEL): $(LOADEROBJECT) $(OBJECTS) $(STATICLIBS) $(OBJECTS_ASM)
	$(LD) $(LDFLAGS) -T kernel.ld -o $(KERNEL) $(LOADEROBJECT) $(OBJECTS) $(OBJECTS_ASM) $(STATICLIBS)
	$(LD) $(LDFLAGS) -T kernel.ld --oformat=elf64-x86-64 -o kernel.elf $(LOADEROBJECT) $(OBJECTS) $(OBJECTS_ASM) $(STATICLIBS)

%.o: %.c
	$(GCC) $(GCCFLAGS) -I./include -DSTACK_SIZE_VALUE=$(STACK_SIZE) -c $< -o $@

%.o : %.asm
	$(ASM) $(ASMFLAGS) $< -o $@

$(LOADEROBJECT):
	$(ASM) $(ASMFLAGS) $(LOADERSRC) -o $(LOADEROBJECT)

clean:
	rm -rf asm/*.o *.o */*.o *.bin *.elf */*/*.o

buddy: clean $(KERNEL)
all: clean $(KERNEL)

.PHONY: all clean buddy