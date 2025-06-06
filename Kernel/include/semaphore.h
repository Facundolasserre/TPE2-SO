#ifndef _SEMAPHORE_H
#define _SEMAPHORE_H

#include <stdint.h>
#include <interrupts.h>
#include <processQueue.h>
#include <scheduler.h>
#include <list.h>

#define SEM_NAME_SIZE 32

typedef List semaphoreList_t;

typedef struct semaphore_t {
    lock_t lock;
    char name[SEM_NAME_SIZE];
    int value;
    processQueueADT blockedQueue;
    int processesCount;
}semaphore_t;

#define MAX_SEMAPHORES (CHUNK_SIZE / sizeof(semaphore_t))

int initSemaphores(void); //Inicializa la lista de semaforos

int addSemaphore(char * name, int initialValue);

void removeSemaphore(char * name);

void semOpen(char *sem_name, uint64_t init_value);

void semClose(char * sem_name);

void semWait(char *sem_name);

void semPost(char *sem_name);

int semCompare(const void * sem_a, const void * sem_b);



#endif