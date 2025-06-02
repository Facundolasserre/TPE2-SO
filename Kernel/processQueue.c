#include <processQueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <memoryManager.h>


typedef struct node * queue_t;

typedef struct queue_CDT{
    queue_t rear;
    size_t size;
}queue_CDT;

typedef struct node{
    processCB pcb; //puntero al proceso
    struct node * next; //puntero al siguiente nodo
    struct node * prev; //puntero al nodo anterior
} node_t;

processQueueADT newProcessQueue() {
    processQueueADT queue = mem_alloc(1, sizeof(processQueueCDT));
    if (queue == NULL) {
        fprintf(stderr, "No se pudo alocar memoria para processQueueADT\n");
        return NULL; 
    }
    queue->size = 0;
    queue->iterator = NULL;
    
    return queue;
}

size_t get_size(processQueueADT queue) {
    return queue->size;
}



void freeProcessQueue(processQueueADT queue) {
    if (queue == NULL || queue->first == NULL){
        mem_free(queue);
        return;
    }
    
    processNode *  current = queue->first;
    processNode * next;
    
    do{
        next = current->next;
        mem_free(current->pcb); //liberar el processCB asociado
        mem_free(current); // liberar el nodo
        current = next;
    }while(current != queue->first);
    
    mem_free(queue);// liberar el ultimo nodo
}




void addProcessToQueue(processQueueADT queue, processCB pcb) {
    queue_t new = mem_alloc(sizeof(node_t));
    if (new == NULL) {
        return;
    }

    queue->size++;
    new->pcb = pcb;

    if (queue->iterator == NULL) {

        queue->iterator = new;
        queue->iterator->next = queue->iterator;
        new->iterator->prev = queue->iterator;
        return;

    }
    // insertar al final de la cola
    queue_t rear = queue->iterator;
    iterator->prev->next = new;
    new->prev = iterator->prev;
    iterator->prev = new;
    new->next = rear;
}




processCB dequeueProcess(processQueueADT queue) {
    if (has_next(queue)){
        if (queue->iterator == queue->iterator->next) {  // Solo un nodo en la cola
            processCB pcb = queue->iterator->pcb;
            mem_free(queue->iterator);
            queue->iterator = NULL;
            return pcb;
        }
        processCB aux = queue->iterator;
        queue->iterator->prev->next = queue->iterator->next;
        queue->iterator->next->prev = queue->iterator->prev;
        queue->iterator = queue->iterator->next;
        queue->size--;
        processCB pcb = aux->pcb;            // Almacenar el PCB antes de liberar
        mem_free(aux);                   // Liberar el nodo eliminado
        return aux->pcb;
    }else {
        return (processCB){0, 0, 0, 0, TERMINATED};
    }
}

processCB find_pid_dequeue(processQueueADT queue, uint64_t pid){

    if(queue->iterator == NULL){
        return (processCB){0,0,0,0,TERMINATED}; //si esta vacia
    }

    queue_t aux = queue->iterator;
    do {
        if (aux->pcb.pid == pid) {
            // Si encontramos el proceso con el pid correspondiente

            processCB found_pcb = aux->pcb;

            if (aux == queue->iterator && queue->iterator->next == queue->iterator) {
                // Solo un nodo en la cola y es el que buscamos
                queue->iterator = NULL;
            } else {
                // Ajustamos los punteros para eliminar el nodo
                aux->prev->next = aux->next;
                aux->next->prev = aux->prev;

                // Si el nodo que estamos eliminando es el rear, lo actualizamos
                if (aux == queue->iterator) {
                    queue->iterator = aux->next;
                }
            }

            queue->size--;
            mem_free(aux);  // Liberamos la memoria del nodo eliminado
            return found_pcb;   // Retornamos el PCB del proceso encontrado
        }

        current = current->next;  // Pasamos al siguiente nodo
    } while (current != queue->iterator);  // Seguimos mientras no volvamos al inicio

    // Si no encontramos el proceso con el pid dado, retornamos un PCB "nulo"
    return (processCB){-1, 0, 0, 0, TERMINATED};
    
}





int hasNextProcess(processQueueADT queue) {
    return queue->iterator != NULL;
}


// void to_begin(processQueueADT queue) {
//     queue->iterator = queue->first;
// }


// char * next(processQueueADT queue) {
//     if (hasNext(queue)) {
       
//         queue->iterator = queue->iterator->next;
//     }else{
//         fprintf(stderr, "No existe el path\n");
//         exit(1);
//     }
    
// }