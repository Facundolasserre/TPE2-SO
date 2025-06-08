#include <scheduler.h>
#include <memoryManager.h>
#include <processQueue.h>
#include <utils.h>
#include <list.h>
#include "../include/semaphore.h"




List * semList;

int sem_compare(void *semA, void *semB) {
    const semaphore_t * sem_a = (const semaphore_t *)semA;
    const char *sem_b_name = (char*)semB;
    return strcmp(sem_a->name, sem_b_name);
}

int initSemaphores(){
    semList = listInit(sem_compare);
    if(semList == NULL){
        return -1;
    }
    return 0;
}

int addSemaphore(char * name, int initialValue) {
    semaphore_t * newSemaphore = (semaphore_t*)mem_alloc(sizeof(semaphore_t));

    if(newSemaphore == NULL){
        return -1;
    }

    strcpy(newSemaphore->name, name, strlen(name));

    newSemaphore->value = initialValue;
    newSemaphore->lock = (lock_t)1;
    newSemaphore->blockedQueue = newProcessQueue();
    newSemaphore->processesCount = 1;

    listAdd(semList, newSemaphore);

    return 0;
}


void removeSemaphore(char * name){
    semaphore_t * foundSemaphore = (semaphore_t *)listGet(semList, name);
    
    listRemove(semList, name);

    mem_free(foundSemaphore);
    
}

int sem_open(char *name, uint64_t init_value){
    semaphore_t * aux = (semaphore_t *)listGet(semList, name);

    if(aux == NULL){
        return addSemaphore(name, init_value);
    }else{
        (aux->processesCount)++;
    }
    return 0;
}

void sem_close(char * name){
    semaphore_t * aux = (semaphore_t *)listGet(semList, name);

    if(aux == NULL){
        return;
    }

    acquire(aux->lock);
    (aux->processesCount)--;
    release(aux->lock);

    if(aux->processesCount == 0){
        removeSemaphore(name);
        return;
    }

    return;
}



void sem_wait(char * name){
    semaphore_t * aux = (semaphore_t *)listGet(semList, name);

    if(aux == NULL){
        return;
    }

    acquire(aux->lock);
    uint8_t toBlock = aux->value <= 0;
    (aux->value)--;
    release(aux->lock);

    if(toBlock){
        block_current_process_to_queue(aux->blockedQueue);
    }
    return;

}

int64_t sem_post(char * name){
    semaphore_t * aux = (semaphore_t *)listGet(semList, name);

    if(aux == NULL){
        return 0;
    }

    acquire(aux->lock);
    uint8_t toUnblock = aux->value < 0;
    (aux->value)++;
    release(aux->lock);

    //desbloqueo el primer proceso bloqueado en la cola del semaforo
    if(toUnblock){
        unblock_process_from_queue(aux->blockedQueue);
    }

    return 1;
}