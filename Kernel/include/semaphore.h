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

int sem_open(char *sem_name, uint64_t init_value);

void sem_close(char * sem_name);

void sem_wait(char *sem_name);

int64_t sem_post(char *sem_name);

int sem_compare(void * sem_a,  void * sem_b);



#endif