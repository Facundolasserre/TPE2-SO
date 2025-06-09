// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
#include <fileDescriptor.h>
#include <scheduler.h>
#include <stdlib.h>
#include <list.h>
#include <memoryManager.h>
#include <keyboard.h>
#include <openFile.h>
#include <processQueue.h>
#include <pipe.h>
#include <videoDriver.h>



List *openFDList;               //Lista global de fd de archivos abiertos
uint64_t currentFDId = 0;       //Id del siguiente fd a crear
uint64_t stdinFDId;             //Id del fd de entrada estándar
openFile_t *openFileSTDIN;      //FD de entrada estándar
openFile_t *openFileSTDOUT;     //FD de salida estándar
openFile_t * openFileNull;      //FD del dispositivo nulo (null device)


int compareFD( void *fdA, void *fdB) {
    openFile_t *fileA = (openFile_t *)fdA;
    uint64_t *idB = (uint64_t *)fdB;
    return fileA->id - *idB;
}


int compareFDById(openFile_t *fd, uint64_t id) {
    return fd->id - id;
}


void initFileDescriptors(){
    openFDList = listInit(compareFD);
    stdinFDId = pipeCreate();
    //se configura el fd de entrada estándar  (ID0)
    uint64_t * idPtr = &stdinFDId;
    openFileSTDIN = (openFile_t *)listGet(openFDList, idPtr);
    //se configura el fd de salida estándar  (ID1)
    openFileSTDOUT = createFD(0, (char(*)(void *))vDriverRead, (int(*)(void *, char))vDriverWrite, (int(*)())vDriverClose);
    //se configura el fd del dispositivo nulo (null device)  (ID2)
    openFileNull = createFD(0, (char(*)(void *))nullRead, (int (*)(void *, char))nullWrite, (int (*)())nullClose);
}


openFile_t *getSTDIN_FD(){
    return openFileSTDIN;
}


openFile_t *getSTDOUT_FD(){
    return openFileSTDOUT;
}


openFile_t * createFD(void * resource, char (*read)(void * src), int (*write)(void * dest, char data), int (*close)()) {
    openFile_t * new_fd = (openFile_t *)mem_alloc(sizeof(openFile_t));
    if(new_fd == NULL){
        return NULL;
    }
    new_fd->id = currentFDId++;
    new_fd->resource = resource;
    new_fd->read = read;
    new_fd->write = write;
    new_fd->close = close;
    new_fd->ref_count = 1;
    return new_fd;
}


uint64_t addFD(void * resource, char (*read)(void *), int (*write)(void *, char), int (*close)()){
    openFile_t *new_fd = createFD(resource, read, write, close);
    if(new_fd == NULL){
        return -1;
    }
    listAdd(openFDList, new_fd);
    return new_fd->id; //se devuelve el id del nuevo fd
}


void removeFD(uint64_t id){
    uint64_t * idPtr = &id;
    openFile_t *found_fd = (openFile_t *)listGet(openFDList, idPtr);
    listRemove(openFDList, found_fd);
    mem_free(found_fd);
}


openFile_t ** openFDTable(uint64_t fdIds[MAX_FD], int fdCount){
    openFile_t **fdTable = (openFile_t **)mem_alloc(sizeof(openFile_t*) * MAX_FD);
    for(int i=0; i < MAX_FD; i++){              //inicializamos la tabla de descriptores de archivos
        fdTable[i] = NULL;
    }
    if(fdIds == NULL || fdCount < 2){           //si no se pasan ids o son menos de 2, usamos los descriptores estándar
        fdTable[0] = getSTDIN_FD();
        fdTable[1] = getSTDOUT_FD();
    }else{                                      
        if(fdIds[0] == STDIN){
            fdTable[0] = getSTDIN_FD();
        }else if(fdIds[0] == DEVNULL){          //si el id es DEVNULL, usamos el dispositivo nulo
            fdTable[0] = openFileNull;
        }else{                                  //si es otro id, buscamos el fd en la lista de descriptores abiertos            
            uint64_t * idPtr = &fdIds[0];
            openFile_t *fd = (openFile_t *)listGet(openFDList, idPtr);
            if(fd == NULL){                    
                goto undo;
            }
            fd->ref_count++;
            fdTable[0] = fd;
        }
        if(fdIds[1] == STDOUT){                 //si el id es STDOUT, usamos el fd de salida estándar
            fdTable[1] = getSTDOUT_FD();
        }else if(fdIds[1] == DEVNULL){          //si el id es DEVNULL, usamos el dispositivo nulo
            fdTable[1] = openFileNull;
        }else{                                  //si es otro id, buscamos el fd en la lista de descriptores abiertos    
            uint64_t *idPtr = &fdIds[1];
            openFile_t * fd = (openFile_t *)listGet(openFDList, idPtr);
            if(fd == NULL){
                goto undo;
            }
            fd->ref_count++;
            fdTable[1] = fd;
        }
    }
    for(int i=2; i<fdCount ; i++){              
        uint64_t * idPtr = &fdIds[i];
        openFile_t *fd = (openFile_t *)listGet(openFDList, idPtr);
        if(fd == NULL){
            goto undo;
        }
        fd->ref_count++;
        fdTable[i] = fd;
    }
    return fdTable;
    
    //si no se pudo crear la tabla de descriptores, liberamos los recursos y devolvemos NULL
    undo:                                      
    for(int i = 0; i < MAX_FD; i++){
        if(fdTable[i] != NULL){
            fdTable[i]->ref_count--;
        }
    }
    return NULL;
}


uint64_t openFD(uint64_t id){
    processCB current = getCurrentProcess();
    uint64_t * idPtr = &id;
    openFile_t *found_fd = (openFile_t *)listGet(openFDList, idPtr);
    if(found_fd == NULL){
        return -1;
    }
    for(int i = 0; i < MAX_FD; i++){        //vemos si ya estaba abierto en la tabla de descriptores del proceso actual
        if(compareFDById(current.fdTable[i], id) == 0){
            return i;
        }
    }
    for(int i = 0; i < MAX_FD; i++){        // buscamos un espacio libre en la tabla de descriptores del proceso actual
        if(current.fdTable[i] == NULL){
            current.fdTable[i] = found_fd;
            found_fd->ref_count++;
            return i;
        }
    }
    return -1;                              //no hay espacio en la tabla de descriptores del proceso actual
}


int closeFD(uint64_t id){
    if(id < 2 || id == stdinFDId){
        return 0;
    }
    uint64_t * idPtr = &id;
    openFile_t *found_fd = (openFile_t *)listGet(openFDList, idPtr);
    if(found_fd == NULL){
        return 0;
    }
    found_fd->ref_count--;
    if(found_fd->ref_count == 0){
        removeFD(id);
    }
    return 1;
}


int closeFDCurrentProcess(uint64_t index) {
    processCB current = getCurrentProcess();
    openFile_t *fdToClose = current.fdTable[index];
    if(fdToClose == NULL) {
        return 0; 
    }
    uint64_t fdId = fdToClose->id;
    current.fdTable[index] = NULL; 
    return closeFD(fdId); 
}


char readFD(uint64_t fdIndex){
    processCB current = getCurrentProcess();
    openFile_t * openFileFD = current.fdTable[fdIndex];
    
    if(openFileFD == NULL){
        return -1;
    }
    return openFileFD->read(openFileFD->resource);
}


int writeFD(uint64_t fdIndex, char data){
    processCB current = getCurrentProcess();
    openFile_t * openFileFD = current.fdTable[fdIndex];
    
    if(openFileFD == NULL){
        return -1;
    }
    return openFileFD->write(openFileFD->resource, data);
}

//Implementación dummy para cerrar el dispositivo nulo (null device)
int nullClose() {
    return 1; 
}

//Implementación dummy para leer desde dispositivo nulo (null device)
char nullRead(void *src) {
    block_process(); 
    return 0;
}

//Implementación dummy para escribir en dispositivo nulo (null device)
int nullWrite(void *src, char data) {
    return 1; 
}













