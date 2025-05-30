#include <processQueue.h>
#include <stdio.h>
#include <stdlib.h>


processQueueADT newProcessQueue() {
    processQueueADT queue = calloc(1, sizeof(processQueueCDT));
    if (queue == NULL) {
        fprintf(stderr, "No se pudo alocar memoria para processQueueADT\n");
        return NULL; 
    }
    queue->first = NULL;
    queue->iterator = NULL;
    queue->last = NULL;
    
    return queue;
}



void freeProcessQueue(processQueueADT queue) {
    if (queue == NULL || queue->first == NULL){
        free(queue);
        return;
    }
    
    processNode *  current = queue->first;
    processNode * next;
    
    do{
        next = current->next;
        free(current->pcb); //liberar el processCB asociado
        free(current); // liberar el nodo
        current = next;
    }while(current != queue->first);
    
    free(queue);// liberar el ultimo nodo
}




void addProcessToQueue(processQueueADT queue, processCB * pcb) {
    processNode * new = (processNode*)malloc(sizeof(processNode));
    if (new == NULL) {
        fprintf(stderr, "No se pudo alocar memoria para processNode\n");
        return;
    }

    new->pcb = pcb;
    new->next = NULL;
    new->prev = NULL;

    if (queue->first == NULL) {
        // Cola vacía
        queue->first = new;
        queue->last = new;
        new->next = new; // Circular
        new->prev = new; // Circular
    } else {
        // insertar al final de la cola
        new->prev = queue->last;
        new->next = queue->first;
        queue->last->next = new;
        queue->first->prev = new;
        queue->last = new; 
    }
}




processCB * dequeueProcess(processQueueADT queue) {
    processNode empty = {0};
    if (queue == NULL || queue->first == NULL) { //si esta vacia
        return &empty; 
    }
    
    processNode * nodeToDequeue = queue->first;
    processCB * pcb = nodeToDequeue->pcb;

   if(queue->first == queue->last) { // solo un nodo en la cola
        queue->first = NULL;
        queue->last = NULL;
    } else {
        queue->first = nodeToDequeue->next;
        queue->last->next = queue->first; //circular
        queue->first->prev = queue->last; 
    }
    free(nodeToDequeue); // libero el nodo
    return pcb; // devuelvo pcb del nodo eliminado
}





void to_begin(processQueueADT queue) {
    queue->iterator = queue->first;
}
int has_next(processQueueADT queue) {
    return queue->iterator != NULL;
}
char * next(processQueueADT queue) {
    if (hasNext(queue)) {
       
        queue->iterator = queue->iterator->next;
    }else{
        fprintf(stderr, "No existe el path\n");
        exit(1);
    }
    
}