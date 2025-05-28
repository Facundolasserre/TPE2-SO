#ifndef PROCESS_H
#define PROCESS_H

#include <stdint.h>
#include "memoryManager.h"

#define READY 1
#define RUNNING 2
#define BLOCKED 3
#define MAX_PROCESSES 100


typedef struct {
    uint64_t pid;      
    void *stack;       // Puntero a la pila
    uint64_t rsp;      // Stack pointer
    uint64_t rip;      // Instruction pointer
    int state;         // READY, RUNNING, BLOCKED
    int priority;      // Prioridad del proceso
} Process;

Process *create_process(int priority, void *entry_point);
void free_process(Process *process);

//Funciones para gestionar procesos
void init_process_list(void);
void add_process(Process *process);
int get_process_list(Process *buffer, int max);

#endif