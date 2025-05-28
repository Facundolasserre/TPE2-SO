#include <processQueue.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct node * queueNode;

typedef struct node{
    ;char * path; 
    size_t size;
    struct node * next;
}node;

typedef struct processQueueCDT{
    queueNode first;
    queueNode iterator;
    queueNode last;
    int (*compare)(size_t, size_t);
}processQueueCDT;

processQueueADT newProcessQueue(int (*compare)(size_t, size_t)) {
    processQueueADT queue = calloc(1, sizeof(processQueueCDT));
    if (!queue) {
        fprintf(stderr, "No se pudo alocar memoria para processQueueADT\n");
        return NULL; 
    }
    queue->first = NULL;
    queue->iterator = NULL;
    queue->last = NULL;
    queue->compare = compare;
    return queue;
}




static queueNode recursiveAdd(queueNode current, char * path, size_t size_mb,  int (*compare)(size_t, size_t)) {
    if (current == NULL || compare(size_mb, current->size) < 0) {
        queueNode newNode = malloc(sizeof(processNode));
        if (!newNode) {
            fprintf(stderr, "No se pudo alocar memoria para el nuevo nodo\n");
            return NULL; 
        }
        newNode->path = path;
        newNode->size = size_mb;
        newNode->next = current;
        return newNode; 
    }

    if(compare(current->size, size_mb) < 0){
        current->next = recursiveAdd(current->next, path, size_mb, compare);
    }
    
    return current; 
}

void add(processQueueADT queue, char * path, size_t size_mb) {
    queueNode newNode = malloc(sizeof(processNode));
    if (!newNode) {
        fprintf(stderr, "No se pudo alocar memoria para el nuevo nodo\n");
        return;
    }
    newNode->path = path;
    newNode->size = size_mb;
    newNode->next = NULL;

    queue->first = recursiveAdd(queue->first, path, newNode, queue->compare);

    free(newNode);
}

void freeProcessQueue(processQueueADT queue) {
    queueNode current = queue->first;
    while (current != NULL) {
        queueNode nextNode = current->next;
        free(current);
        current = nextNode;
    }
    free(queue);
}


void to_begin(processQueueADT queue) {
    queue->iterator = queue->first;
}
int has_next(processQueueADT queue) {
    return queue->iterator != NULL;
}
char * next(processQueueADT queue) {
    if (hasNext(queue)) {
        char * path = queue->iterator->path;
        queue->iterator = queue->iterator->next;
        return path;
    }else{
        fprintf(stderr, "No existe el path\n");
        exit(1);
    }
    
}