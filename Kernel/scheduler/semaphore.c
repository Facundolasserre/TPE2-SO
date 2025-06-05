#include <scheduler.h>
#include <semaphore.h>
#include <memoryManager.h>
#include <processQueue.h>
#include <utils.h>


typedef struct semaphoreList_t{
    semaphore_t semaphore;
    struct semaphoreList_t *next;
} semaphoreList_t;

semaphoreList_t * semList = NULL;


semaphoreList_t * add_semaphore(semaphoreList_t **head, char * name, int initialValue) {
    semaphoreList_t * newSemaphore = (semaphoreList_t*)mem_alloc(sizeof(semaphoreList_t));

    strcpy(newSemaphore->semaphore.name, name, strlen(name));

    newSemaphore->semaphore.value = initialValue;
    newSemaphore->semaphore.lock = 0;
    newSemaphore->semaphore.blockedQueue = newProcessQueue();

    if (*head == NULL) {
        *head = newSemaphore;
    } else {
        semaphoreList_t *aux = *head;
        while (aux->next != NULL) {
            aux = aux->next;
        }
        aux->next = newSemaphore;
    }

    return newSemaphore;
}


void remove_sem(semaphoreList_t **head, char * name){
    semaphoreList_t *temp = *head;
    semaphoreList_t *prev = NULL;

    if (temp != NULL && strcmp(temp->semaphore.name, name) == 0) {
        *head = temp->next;
        freeProcessQueue(temp->semaphore.blockedQueue);
        mem_free(temp);
        return;
    }

    while (temp != NULL && strcmp(temp->semaphore.name, name) != 0) {
        prev = temp;
        temp = temp->next;
    }

    if (temp == NULL) {
        return;
    }

    prev->next = temp->next;

    while(hasNextProcess(temp->semaphore.blockedQueue)){
        unblock_process_from_queue(temp->semaphore.blockedQueue);   
    }
    freeProcessQueue(temp->semaphore.blockedQueue);
    mem_free(temp);
}


semaphoreList_t* find_sem(char * sem_name){
    semaphoreList_t * aux = semList;
    while(aux != NULL){
        if(strcmp(aux->semaphore.name, sem_name) == 0)
            return aux;
        aux = aux->next;
    }
    return NULL;
}


int64_t sem_open(char *sem_name, uint64_t init_value){
    semaphoreList_t * aux = find_sem(sem_name);
    if(aux == NULL)
        aux = add_semaphore(&semList, sem_name, init_value);
    acquire(aux->semaphore.lock);
    aux->semaphore.value++;
    release(aux->semaphore.lock);
    return 0;
}

int64_t sem_close(char * sem_name){
    semaphoreList_t * aux = find_sem(sem_name);
    if(aux != NULL) {
        acquire(aux->semaphore.lock);
        if(aux->semaphore.value > 0)
            aux->semaphore.value--;
        else
            remove_sem(&semList, sem_name);
        release(aux->semaphore.lock);
    }
    return 0;
}

void sem_wait(char *sem_name){
    semaphoreList_t * sem_node = find_sem(sem_name);
    if(sem_node == NULL) return;
    acquire(sem_node->semaphore.lock);
    if(sem_node->semaphore.value > 0){
        (sem_node->semaphore.value)--;
    } else {
        block_current_process_to_queue(sem_node->semaphore.blockedQueue);
    }
    release(sem_node->semaphore.lock);
}

int64_t sem_post(char *sem_name){
    semaphoreList_t * sem_node;
    if((sem_node = find_sem(sem_name)) == NULL)
        return 1;

    acquire(sem_node->semaphore.lock);
    if (sem_node->semaphore.value == 0){
        unblock_process_from_queue(sem_node->semaphore.blockedQueue);
    }
    (sem_node->semaphore.value)++;
    release(sem_node->semaphore.lock);
    return 0;
}