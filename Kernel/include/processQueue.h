#ifndef PROCESS_QUEUE_H
#define PROCESS_QUEUE_H

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <fileDescriptor.h>
#include <openFile.h>
#include <list.h>


typedef struct processQueueCDT * processQueueADT;

//process control block (informacion de un proceso)
typedef struct processCB{ 
    int pid; 
    void * base_pointer; //puntero a la base del stack
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
    openFile_t * fdTable[MAX_FD];
    processQueueADT waitingQueue;
} processCB;


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

processCB returnNullProcess();

#endif 