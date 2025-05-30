#ifndef PROCESS_H
#define PROCESS_H

#include <stdint.h>
#include "memoryManager.h"
#include "processQueue.h"

#define READY 1
#define RUNNING 2
#define BLOCKED 3
#define TERMINATED 4
#define MAX_PROCESSES 100


typedef struct {
    uint64_t pid;      
    void *stack;       // Puntero a la pila
    uint64_t rsp;      // Stack pointer
    uint64_t rip;      // Instruction pointer
    int state;         // READY, RUNNING, BLOCKED
    int priority;      // Prioridad del proceso
} Process;


Process *createProcess(int priority, void *entry_point, processQueueADT queue,  uint64_t argc, char *argv[]);

void freeProcess(Process *process);

//Funciones para gestionar procesos
void initProcessList(void);
void addProcess(Process *process);
int getProcessList(Process *buffer, int max);


#endif