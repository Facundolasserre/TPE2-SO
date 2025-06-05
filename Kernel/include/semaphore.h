#ifndef _SEMAPHORE_H
#define _SEMAPHORE_H

#include <stdint.h>
#include <interrupts.h>
#include <processQueue.h>
#include <scheduler.h>

#define SEM_NAME_SIZE 32

typedef struct {
    lock_t lock;
    char name[SEM_NAME_SIZE];
    int value;
    processQueueADT blockedQueue;
} semaphore_t;

#define MAX_SEMAPHORES (CHUNK_SIZE / sizeof(semaphore_t))

int64_t sem_open(char *sem_id, uint64_t initialValue);
int64_t sem_close(char * sem_id);
void sem_wait(char * sem_name);
int64_t sem_post(char *sem_id);

#endif