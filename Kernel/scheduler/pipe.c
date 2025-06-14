// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
#include <semaphore.h>
#include <list.h>
#include <stdlib.h>
#include <pipe.h>
#include <stdint.h>
#include <memoryManager.h>
#include <fileDescriptor.h>
#include <utils.h>
#include <scheduler.h>

List * pipesList;               // Lista global de pipes
uint16_t currentPipeId = 0;     // Id del siguiente pipe a crear


int initPipes(){
    pipesList = listInit((int (*)(void *, void *))comparePipes);
    if (pipesList == NULL) {
        return -1;
    }
    return 0;
}


uint64_t pipeCreate(){
    pipe * newPipe = pipeInit();
    if(newPipe == NULL){
        return -1;
    }
    uint64_t pipe_fd_id = addFD((void *)(uintptr_t)newPipe->id, (char (*)(void *))pipeRead, (int (*)(void *, char))pipeWrite, (int (*)())pipeDestroy);
    if(pipe_fd_id == -1){
        freePipe(newPipe);
        return -1;
    }
    listAdd(pipesList, newPipe);
    return pipe_fd_id;
}


void pipeDestroy(uint16_t pipeId){
    pipe target = {.id = pipeId};
    pipe * found = (pipe *)listGet(pipesList, &target);
    if(found == NULL){
        return;
    }
    listRemove(pipesList, found);
    freePipe(found);
    return;
}


char pipeRead(uint16_t pipeId){
    pipe target = {.id = pipeId};
    pipe * found = (pipe *)listGet(pipesList, &target);
    if(found == NULL){
        return '\0';
    }
    sem_wait(found->sem_name_data_available);
    sem_wait(found->sem_name_mutex);
    char c = found->buffer[found->readIndex];
    found->readIndex = (found->readIndex + 1) % BUFFER_SIZE;
    sem_post(found->sem_name_mutex);
    return c;
}


int pipeWrite(uint16_t pipeId, char c){
    pipe target = {.id = pipeId};
    pipe * found = (pipe *)listGet(pipesList, &target);
    if(found == NULL){
        return 0;
    }
    sem_wait(found->sem_name_mutex);
    found->buffer[found->writeIndex] = c;
    found->writeIndex = (found->writeIndex + 1) % BUFFER_SIZE;
    sem_post(found->sem_name_mutex);
    sem_post(found->sem_name_data_available);
    return 1;
}


int comparePipes(void *a, void *b){
    const pipe *pipeA = (const pipe *)a;
    const pipe *pipeB = (const pipe *)b;
    return (pipeA->id - pipeB->id);
}


pipe * pipeInit(){
    pipe * newPipe = (pipe *)mem_alloc(sizeof(pipe));
    if(newPipe == NULL){
        return NULL;
    }  
    newPipe->buffer = (char *)mem_alloc(BUFFER_SIZE * sizeof(char));
    if(newPipe->buffer == NULL){
        mem_free(newPipe);
        return NULL;
    }
    newPipe->id = currentPipeId++;
    newPipe->readIndex = 0;
    newPipe->writeIndex = 0;
    newPipe->sem_name_data_available = (char *)mem_alloc(SEMAPHORE_NAME_SIZE * sizeof(char));
    getSemaphoreName("pipe_sem", newPipe->id, newPipe->sem_name_data_available);
    sem_open(newPipe->sem_name_data_available, 0);
    newPipe->sem_name_mutex = (char *)mem_alloc(SEMAPHORE_NAME_SIZE * sizeof(char));
    getSemaphoreName("pipe_mutex", newPipe->id, newPipe->sem_name_mutex);
    sem_open(newPipe->sem_name_mutex, 1);
    return newPipe;
}


void freePipe(pipe * p){
    if(p == NULL) {
        return;
    } 
    mem_free(p->buffer);
    sem_close(p->sem_name_data_available);
    sem_close(p->sem_name_mutex);
    mem_free(p->sem_name_data_available);
    mem_free(p->sem_name_mutex);
    mem_free(p);
}

void getSemaphoreName(const char * baseName, int id, char * dest){
    int baseLength = 0;
    while(baseName[baseLength] != '\0') {
        dest[baseLength] = baseName[baseLength];
        baseLength++;
    }
    char idStr[10];
    intToStr(id, idStr);
    int idIndex = 0;
    while(idStr[idIndex] != '\0') {
        dest[baseLength++] = idStr[idIndex++];
    }
    dest[baseLength] = '\0';
}


//------------------------------Sección de pruebas de pipes------------------------------------------
#include <videoDriver.h>

int status_writer1 = 0;     // Estado del escritor 1
int status_writer2 = 0;     // Estado del escritor 2
int status_reader = 0;      // Estado del lector
#define PIPE_ID 0           // ID del pipe usado


// Función para escribir 'A' en el pipe (writer1)
void pipe_writer1() {
    char c = 'A'; 
    while (1) {
        vDriver_prints("WRITTEN A", BLACK, WHITE);
        if (pipeWrite(PIPE_ID, c) == 0) {
            status_writer1 = 1; 
        } else {
            status_writer1 = 0; 
        }
    }
}


// Función para escribir 'B' en el pipe (writer2)
void pipe_writer2() {
    char c = 'B'; 
    while (1) {
        vDriver_prints("WRITTEN B", WHITE, BLACK);
        if (pipeWrite(PIPE_ID, c) == 0) {
            status_writer2 = 1; 
        } else {
            status_writer2 = 0; 
        }
    }
}


// Función para leer del pipe
void pipe_reader() {
    while (1) {
        char c = pipeRead(PIPE_ID);
        vDriver_print(c, WHITE, BLACK); 
        if (c != -1) { 
            status_reader = 1; 
        } else {
            status_reader = 0; 
        }
    }
}
    

void pipes_test() {
    int pipe_id = pipeCreate();
    if (pipe_id == -1) {
        return;
    }
    createProcess(0, (program_t)pipe_writer1, 0, NULL, NULL, 0); // Crea el proceso writer1
    createProcess(0, (program_t)pipe_writer2, 0, NULL, NULL, 0); // Crea el proceso writer2
    createProcess(0, (program_t)pipe_reader, 0, NULL, NULL, 0);  // Crea el proceso reader
}