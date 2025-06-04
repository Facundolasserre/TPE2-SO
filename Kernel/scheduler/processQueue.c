#include <processQueue.h>
#include <stdio.h>
#include <memoryManager.h>

typedef struct node * queue_t;

typedef struct node{
    processCB pcb; //puntero al proceso
    struct node * next; //puntero al siguiente nodo
    //struct node * prev; //puntero al nodo anterior
} node_t;

typedef struct processQueueCDT{
    queue_t rear;
    size_t size;
} processQueueCDT;

processQueueADT newProcessQueue() {
    processQueueADT queue = mem_alloc(sizeof(processQueueCDT));
    if (queue == NULL) {
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
    if(queue->rear == NULL){
        mem_free(queue); // si la cola está vacia simlemente liberamos la memoria del objeto cola
        return;
    }

    queue_t current = queue->rear->next;
    queue_t next;

    while (current != queue->rear) {
        next = current->next; // guardamos el siguiente nodo
        mem_free(current); // liberamos el nodo actual
        current = next; // pasamos al siguiente nodo
    }

    mem_free(queue->rear); // liberamos el último nodo
    mem_free(queue); // liberamos la memoria del objeto cola
    
}




void addProcessToQueue(processQueueADT queue, processCB pcb) {
    queue_t new = mem_alloc(sizeof(node_t));
    if (new == NULL) {
        return;
    }

    new->pcb = pcb;

    if (queue->rear == NULL) {
        new-> next = new; //si la cola esta vacia, el nuevo y unico nodo se apunta a si mismo
    }else{
        new->next = queue->rear->next; // El nuevo nodo apunta al primer nodo
        queue->rear->next = new; // El nodo actual apunta al nuevo nodo
    }
    
    queue->rear = new; // El nuevo nodo se convierte en el último nodo
    queue->size++;
}




processCB dequeueProcess(processQueueADT queue) {
    if (queue->rear == NULL) {
        return (processCB){-1, 0, 0, 0, 0, TERMINATED}; // Si la cola está vacía
    }
    processCB pcb = queue->rear->next->pcb; // guardamoS el PCB del primer nodo

    if(queue->rear->next == queue->rear) { // si solo hay un nodo en la cola
        mem_free(queue->rear); // liberamos el único nodo
        queue->rear = NULL; // la cola queda vacía
    } else {
        queue_t aux = queue->rear->next; // guardamos el primer nodo
        queue->rear->next = queue->rear->next->next; // el último nodo apunta al segundo nodo
        mem_free(aux); // liberamos el primer nodo
    }

    queue->size--; 
    return pcb; 

}




processCB find_pid_dequeue(processQueueADT queue, uint64_t pid){

    if(queue->rear == NULL){
        return (processCB){-1, 0, 0, 0, 0, TERMINATED}; //si esta vacia devuelve un pcb null
    }

    queue_t current = queue->rear;

    do {
        if (current->next->pcb.pid == pid) { 
            processCB found_pcb = current->next->pcb;

            if (current->next == queue->rear) { // Si el nodo a eliminar es el único nodo
                
                if(queue->rear->next == queue->rear){//si la cola tiene un solo nodo
                    mem_free(queue->rear);
                    queue->rear = NULL;  
                }else {
                    queue_t to_delete = queue->rear; // Nodo a eliminar

                    while(to_delete->next != queue->rear) { 
                        to_delete = to_delete->next;
                    }

                    to_delete->next = queue->rear->next; 
                    mem_free(queue->rear);
                    queue->rear = to_delete; // Actualizamos el último nodo
                }
            
            } else{
                queue_t to_delete = current->next; // Nodo a eliminar
                current->next = current->next->next; // El nodo anterior apunta al siguiente nodo
                mem_free(to_delete); 
            }
            queue->size--; // Reducimos el tamaño de la cola
            return found_pcb;;
        }
         current = current->next;  // Pasamos al siguiente nodo
    } while (current != queue->rear);  // Seguimos mientras no volvamos al inicio

    // Si no encontramos el proceso con el pid dado, retornamos un PCB "nulo"
    return (processCB){-1, 0, 0, 0, 0, TERMINATED};
    
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