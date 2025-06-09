#include <list.h>
#include <stdlib.h>
#include <memoryManager.h>

List * listInit(int (*compare) (void *,  void *)){
    List * list = (List *)mem_alloc(sizeof(List));
    if (!list) {
        return NULL; // Memory allocation failed
    }
    list->head = NULL;
    list->size = 0;
    list->compare = compare;
    return list;
}

void listAdd(List * list, void * data){
    Node * newNode = (Node *)mem_alloc(sizeof(Node));
    if (!newNode) {
        return; // Memory allocation failed
    }
    newNode->data = data;
    newNode->next = NULL;

    if (list->head == NULL) {
        list->head = newNode;
    } else {
        Node * current = list->head;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = newNode;
    }
    list->size++;
}

int listRemove(List * list, void * data){
    if(list->head == NULL){
        return 0;
    }

    Node * current = list->head;
    Node * previous = NULL;

    while (current != NULL) {
        if (list->compare(current->data, data) == 0) { // Assuming compare returns 0 for equality
            if (previous == NULL) {
                list->head = current->next; // Remove head
            } else {
                previous->next = current->next; // Bypass current node
            }
            mem_free(current); // Free the node memory
            list->size--;
            return 1; // Successfully removed
        }
        previous = current;
        current = current->next;
    }
    return 0; // Data not found in the list
}

void * listGet(List * list, void * data){
    Node * current = list->head;
    while (current != NULL) {
        if (list->compare(current->data, data) == 0) { // Assuming compare returns 0 for equality
            return current->data; // Return the found data
        }
        current = current->next;
    }
    return NULL; // Data not found
}

void listFree(List * list){
    Node * current = list->head;

    while (current != NULL) {
        Node * nextNode = current->next;
        mem_free(current);
        current = nextNode;
    }
    mem_free(list);
}