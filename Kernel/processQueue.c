#include <processQueue.h>
#include <stdio.h>
#include <memoryManager.h>

typedef struct node * queue_t;

typedef struct node{
    processCB pcb; //puntero al proceso
    struct node * next; //puntero al siguiente nodo
    struct node * prev; //puntero al nodo anterior
} node_t;

typedef struct processQueueCDT{
    queue_t rear;
    size_t size;
} processQueueCDT;

processQueueADT newProcessQueue() {
    processQueueADT queue = mem_alloc(sizeof(processQueueCDT));
    if (queue == NULL) {
        fprintf(stderr, "No se pudo alocar memoria para processQueueADT\n");
        return NULL; 
    }
    queue->size = 0;
    queue->rear = NULL;
    
    return queue;
}

size_t get_size(processQueueADT queue) {
    return queue->size;
}



void freeProcessQueue(processQueueADT queue) {
    
    while(queue->rear != NULL){
            queue_t temp = queue->rear->next;
        
        if(queue->rear == queue->rear->next) { // Solo un nodo en la cola
            mem_free(queue->rear);
            queue->rear = NULL;
        } else {
            queue->rear->next = temp->next;
            temp->next->prev = queue->rear;
            mem_free(temp); // Liberar el nodo eliminado
        }
    }
    mem_free(queue); // Liberar la cola
}




void addProcessToQueue(processQueueADT queue, processCB pcb) {
    queue_t new = mem_alloc(sizeof(node_t));
    if (new == NULL) {
        return;
    }

    queue->size++;
    new->pcb = pcb;

    if (queue->rear == NULL) {

        queue->rear = new;
        queue->rear->next = queue->rear;
        queue->rear->prev = queue->rear;
        return;

    }
    // insertar al final de la cola
    queue_t rear = queue->rear;
    rear->prev->next = new;
    new->prev = rear->prev;
    rear->prev = new;
    new->next = rear;
}




processCB dequeueProcess(processQueueADT queue) {
    if (hasNextProcess(queue)){
        if (queue->rear == queue->rear->next) {  // Solo un nodo en la cola
            processCB pcb = queue->rear->pcb;
            mem_free(queue->rear);
            queue->rear = NULL;
            return pcb;
        }
        queue_t aux = queue->rear;
        queue->rear->prev->next = queue->rear->next;
        queue->rear->next->prev = queue->rear->prev;
        queue->rear = queue->rear->next;
        queue->size--;
        processCB pcb = aux->pcb;            // Almacenar el PCB antes de liberar
        mem_free(aux);                   // Liberar el nodo eliminado
        return aux->pcb;
    }else {
        return (processCB){0, 0, 0, 0, TERMINATED};
    }
}

processCB find_pid_dequeue(processQueueADT queue, uint64_t pid){

    if(queue->rear == NULL){
        return (processCB){0,0,0,0,TERMINATED}; //si esta vacia
    }

    queue_t aux = queue->rear;
    do {
        if (aux->pcb.pid == pid) {
            // Si encontramos el proceso con el pid correspondiente

            processCB found_pcb = aux->pcb;

            if (aux == queue->rear && queue->rear->next == queue->rear) {
                // Solo un nodo en la cola y es el que buscamos
                queue->rear = NULL;
            } else {
                // Ajustamos los punteros para eliminar el nodo
                aux->prev->next = aux->next;
                aux->next->prev = aux->prev;

                // Si el nodo que estamos eliminando es el rear, lo actualizamos
                if (aux == queue->rear) {
                    queue->rear = aux->next;
                }
            }

            queue->size--;
            mem_free(aux);  // Liberamos la memoria del nodo eliminado
            return found_pcb;   // Retornamos el PCB del proceso encontrado
        }

        aux = aux->next;  // Pasamos al siguiente nodo
    } while (aux != queue->rear);  // Seguimos mientras no volvamos al inicio

    // Si no encontramos el proceso con el pid dado, retornamos un PCB "nulo"
    return (processCB){-1, 0, 0, 0, TERMINATED};
    
}





int hasNextProcess(processQueueADT queue) {
    return queue->rear != NULL;
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