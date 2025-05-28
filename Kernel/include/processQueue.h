#ifndef __PROCESS_QUEUE_H__
#define __PROCESS_QUEUE_H__

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct processQueueCDT * processQueueADT;

typedef struct processNode{
    uint64_t pid; 
    uint64_t priority; 
    uint64_t rsp;
    enum {
        RUNNING,
        READY,
        BLOCKED,
        TERMINATED
    } state;

} processNode;



processQueueADT newProcessQueue(int (*compare)(size_t, size_t));

void addProcess(processQueueADT queue, char * path, size_t size_mb);

void freeProcessQueue(processQueueADT queue);

void toBeginProcess(processQueueADT queue);
int hasNextProcess(processQueueADT queue);
processNode * nextProcess(processQueueADT queue);

#endif 