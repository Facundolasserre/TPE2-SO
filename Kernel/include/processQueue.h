#ifndef __PROCESS_QUEUE_H__
#define __PROCESS_QUEUE_H__

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


//process control block (informacion de un proceso)
typedef struct processCB{ 
    uint64_t pid; 
    uint64_t rsp;
    int assignedQuantum;
    int usedQuantum;
    enum {
        RUNNING,
        READY,
        BLOCKED,
        TERMINATED
    } state;

} processCB;


typedef struct node * queueNode; //puntero a un nodo de la cola
// estructura de nodo de la cola de procesos
typedef struct node{
    processCB * pcb; //punteo al process control block 
    struct node * prev; //puntero al nodo anterior
    struct node * next; // puntero al nodo siguiente
} processNode;


//estructura propia de la cola
typedef struct processQueueCDT{
    queueNode first; //puntero al primer nodo de la cola
    queueNode iterator; //puntero al nodo actual de la iteracion
    queueNode last; //puntero al ultimo nodo de la cola
    int (*compare)(size_t, size_t); //funcion de comparacion para ordenar los nodos
} processQueueCDT;
typedef struct processQueueCDT * processQueueADT;


//crea una nueva cola de procesos, devuelve la cola inicializada o NULL si no se pudo alocar memoria
processQueueADT newProcessQueue();

//libera la memoria de la cola y de sus nodos
void freeProcessQueue(processQueueADT queue);



//agrega el proceso pcb a la cola queue
void addProcessToQueue(processQueueADT queue, processCB * pcb);



void toBeginProcess(processQueueADT queue); //coloca el iterador al principio de la cola
int hasNextProcess(processQueueADT queue);//verifica si la cola tiene mas procesos y devuelve 1 si tiene, 0 si no tiene
processCB * nextProcess(processQueueADT queue); // devuelve el siguiente proceso de la cola y avanza el iterador

processCB * dequeueProcess(processQueueADT queue); // saca el primer proceso de la cola y lo devuelve, si no hay procesos devuelve NULL

#endif 