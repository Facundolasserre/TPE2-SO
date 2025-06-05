#include <semaphore.h>
#include <list.h>
#include <stdlib.h>
#include <pipe.h>
#include <stdint.h>
#include <memoryManager.h>

List * pipesList;
uint16_t currentPipeId = 0;

int initPipes(){
    pipesList = listInit(comparePipes);
    if (!pipesList) {
        return -1;
    }
    return 0;
}

int pipeCreate(){
    pipe * newPipe = pipeInit();
    if(newPipe == NULL){
        return -1;
    }
    listAdd(pipesList, newPipe);
    return newPipe->id;
}

int pipeDestroy(uint16_t pipeId){
    
}

char pipeRead(uint16_t pipeId);

int pipeWrite(uint16_t pipeId, char c);

int comparePipes(const void *a, const void *b);

pipe * pipeInit();

void freePipe(pipe * p);

void getSemaphoreName(const char * baseName, int id, char * dest);