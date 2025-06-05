#ifndef PIPE_H
#define PIPE_H

#include <stdint.h>
#include <semaphore.h>

#define BUFFER_SIZE 1024
#define SEMAPHORE_NAME_SIZE 32

typedef struct pipe{
    uint16_t id;
    char * buffer;
    uint16_t readIndex;
    uint16_t writeIndex;
    char * sem_name_data_available;
    char * sem_name_mutex;
}pipe;

int initPipes();

int pipeCreate();

int pipeDestroy(uint16_t pipeId);

char pipeRead(uint16_t pipeId);

int pipeWrite(uint16_t pipeId, char c);

int comparePipes(const void *a, const void *b);

pipe * pipeInit();

void freePipe(pipe * p);

void getSemaphoreName(const char * baseName, int id, char * dest);

#endif