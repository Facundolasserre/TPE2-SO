#include <scheduler.h>
#include <memoryManager.h>
#include <processQueue.h>
#include "process.h"

#define MAX_PROCESSES 100

struct Process processes[MAX_PROCESSES];
int currentProcess = -1;
uint64_t currentPID = 0;


void initProcessWrapper(program_t entryPoint, uint64_t argc, char *argv) {
    
}

uint64_t schedule(uint64_t rsp) {
   
}