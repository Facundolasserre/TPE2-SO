//TODAVIA NO EXISTE EL STRUCT PROCESS HASTA EL MOMENTO ASI QUE ESPEREMOS NO LO
//VOY A BORRAR TODAVIA


#ifndef PROCESS_H
#define PROCESS_H

#include <stdint.h>
#include "memoryManager.h"
#include "processQueue.h"
//los defines estan en processQueue.h en la definicion de processCB.
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



#endif