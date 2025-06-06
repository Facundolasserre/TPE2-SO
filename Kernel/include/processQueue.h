#ifndef __PROCESS_QUEUE_H__
#define __PROCESS_QUEUE_H__

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <fileDescriptor.h>

#define MAX_FD 10 // Maximo de file descriptors que puede tener un proceso

//process control block (informacion de un proceso)
typedef struct processCB{ 
    int pid; 
    void *rsp;
    uint8_t priority;
    uint8_t assignedQuantum;
    uint8_t usedQuantum;
    enum {
        RUNNING,
        READY,
        BLOCKED,
        TERMINATED,
        HALT
    } state;
    openFile_t *fds[MAX_FD]; //puntero a los file descriptors del proceso

} processCB;

typedef struct processQueueCDT * processQueueADT;

size_t get_size(processQueueADT queue);

processCB find_pid_dequeue(processQueueADT queue, uint64_t pid);

//crea una nueva cola de procesos, devuelve la cola inicializada o NULL si no se pudo alocar memoria
processQueueADT newProcessQueue();

//libera la memoria de la cola y de sus nodos
void freeProcessQueue(processQueueADT queue);



//agrega el proceso pcb a la cola queue
void addProcessToQueue(processQueueADT queue, processCB pcb);



void toBeginProcess(processQueueADT queue); //coloca el iterador al principio de la cola
int hasNextProcess(processQueueADT queue);//verifica si la cola tiene mas procesos y devuelve 1 si tiene, 0 si no tiene
processCB nextProcess(processQueueADT queue); // devuelve el siguiente proceso de la cola y avanza el iterador

processCB dequeueProcess(processQueueADT queue); // saca el primer proceso de la cola y lo devuelve, si no hay procesos devuelve NULL

#endif 