#include "process.h"
#include "scheduler.h"
#include "processQueue.h"

static int  PID = 0;

Process *createProcess(int priority, void *entry_point, processQueueADT queue,  uint64_t argc, char *argv[]) {
    uint64_t stackPointer = (uint64_t)mem_alloc(STACK_SIZE);

    fill_stack(stackPointer,&initProcessWrapper, entry_point, argc, argv);

    Process *process = {PID++, stackPointer,    QUANTUM,    0,    READY};
                       // PID,    RSP,  assigned_quantum, used_quantum, state
    addProcessToQueue(queue, process);
}



void addProcess(Process *process) {
    
}

int getProcessList(Process *buffer, int max) {
   
}



void freeProcess(Process *process) {
    
}