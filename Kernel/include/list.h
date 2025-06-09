#ifndef _LIST_H_
#define _LIST_H_

#include <stdint.h>

typedef struct Node {
    void * data;
    struct Node * next;
} Node;

typedef struct{
    Node * head;
    uint64_t size;
    int (*compare) ( void *, void *);
}List;

List * listInit(int (*compare) ( void *,  void *));

void listAdd(List * list, void * data);

int listRemove(List * list, void * data);

void * listGet(List * list, void * data);

void listFree(List * list);

#endif