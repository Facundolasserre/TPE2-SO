// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
#include <stdint.h>
#include <string.h>
#include <lib.h>
#include <moduleLoader.h>
#include <naiveConsole.h>
#include <videoDriver.h>
#include <keyboard.h>
#include <idtLoader.h>
#include <time.h>
#include <interrupts.h>
#include <scheduler.h>
#include <memoryManager.h>
#include <pipe.h>
#include <fileDescriptor.h>

#define MEM_SIZE 128*(1024*8)


extern uint8_t text;
extern uint8_t rodata;
extern uint8_t data;
extern uint8_t bss;
extern uint8_t endOfKernelBinary;
extern uint8_t endOfKernel;

extern void _hlt();

static const uint64_t PageSize = 0x1000;

static void * const sampleCodeModuleAddress = (void*)0x400000;
static void * const sampleDataModuleAddress = (void*)0x500000;
static void * const memoryManagerModuleAddress = (void*)0x300000; //definimos direccion donde trabajara el memorymanager

typedef int (*EntryPoint)();


void clearBSS(void * bssAddress, uint64_t bssSize){
	memset(bssAddress, 0, bssSize);
}

void * getStackBase(){
	return (void*)(
		(uint64_t)&endOfKernel
		+ PageSize * 8				//The size of the stack itself, 32KiB
		- sizeof(uint64_t)			//Begin at the top of the stack
	);
}

void * initializeKernelBinary(){
	void * moduleAddresses[] = {
		sampleCodeModuleAddress,
		sampleDataModuleAddress,
	};

	loadModules(&endOfKernelBinary, moduleAddresses);

	clearBSS(&bss, &endOfKernel - &bss);
	
	return getStackBase();
}

//Testeo de procesos
void testProcess1(){
	while(1){
		vDriver_prints("ONE", WHITE, BLACK);
	}
}
void testProcess2(){
	while(1){
		vDriver_prints("TWO", WHITE, BLACK);
	}
}
void testProcess3(){
	while(1){
		vDriver_prints("THREE", WHITE, BLACK);
	}
}

int main()
{	
	_cli();
	load_idt();

	mem_init(memoryManagerModuleAddress, MEM_SIZE);

	initSemaphores();
	initPipes();	
	initFileDescriptors();
	initKeyboard();
	initScheduler();
	
	createProcess(0, sampleCodeModuleAddress, 0, NULL, NULL, 0);

	_sti();

    while(1) {
        _hlt();    
    }
    return 0;
}

