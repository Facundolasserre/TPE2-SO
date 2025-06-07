
#include <fileDescriptor.h>
#include <scheduler.h>
#include <stdlib.h>
#include <list.h>
#include <memoryManager.h>
#include <keyboard.h>
#include <openFile.h>
#include <processQueue.h>
#include <pipe.h>





List *openFDList;
uint64_t currentFDId = 0;
openFile_t *openFileSTDIN = NULL;
openFile_t *openFileSTDOUT = NULL;


int compareFD(void *fdA,  void *fdB) {
    openFile_t *fileA = (openFile_t *)fdA;
    uint64_t idB = (uint64_t)fdB;
    return fileA->id - idB;
}

int compareFDById(openFile_t *fd, uint64_t id) {
    return fd->id - id;
}



void initFileDescriptors(){
    openFDList = list_init(compareFD);
    
    uint64_t stdinFDId = pipeCreate();
    
    openFDList = (openFile_t *)listGet(openFDList, stdinFDId);
    
}




openFile_t *getSTDIN_FD(){
    return openFileSTDIN;
}

openFile_t *getSTDOUT_FD(){
    return openFileSTDOUT;
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




openFile_t * openFDTable(uint64_t fdIds[MAX_FD], int fdCount){
    openFile_t **fdTable = (openFile_t **)mem_alloc(sizeof(openFile_t*) * MAX_FD);
    
    for(int i=0; i < MAX_FD; i++){
        fdTable[i] = NULL;
    }

    if(fdCount < 2){
        fdTable[0] = getSTDIN_FD();
        fdTable[1] = getSTDOUT_FD();
    }else{
        if(fdIds[0] == STDIN){
            fdTable[0] = getSTDIN_FD();
        }else{
            openFile_t *fd = (openFile_t *)listGet(openFDList, fdIds[0]);
            if(fd == NULL){
                goto undo;
            }

            fd->ref_count++;
            fdTable[0] = fd;
        }
        if(fdIds[1] == STDOUT){
            fdTable[1] = getSTDOUT_FD();
        }else{
            openFile_t * fd = (openFile_t *)listGet(openFDList, fdIds[1]);
            if(fd == NULL){
                goto undo;
            }

            fd->ref_count++;
            fdTable[1] = fd;
        }
    }

    for(int i=2; i<fdCount ; i++){
        openFile_t *fd = (openFile_t *)listGet(openFDList, fdIds[i]);

        if(fd == NULL){
            goto undo;
        }

        fd->ref_count++;
        fdTable[i] = fd;
    }

    return fdTable;

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

    openFile_t *found_fd = (openFile_t *)listGet(openFDList, id);
    
    if(found_fd == NULL){
        return NULL;
    }

    //vemos si ya estaba abierto en la tabla de descriptores del proceso actual
    for(int i = 0; i < MAX_FD; i++){
        if(compareFDById(current.fdTable[i], id) == 0){
            return i;
        }
    }

    //si no estaba abierto, buscamos un espacio libre en la tabla de descriptores del proceso actual
    for(int i = 0; i < MAX_FD; i++){
        if(current.fdTable[i] == NULL){
            current.fdTable[i] = found_fd;
            found_fd->ref_count++;
            return i;
        }
    }
    return -1; //no hay espacio en la tabla de descriptores del proceso actual
}




int closeFD(uint64_t id){

    if(id < 2){
        return 0;
    }

    openFile_t *found_fd = (openFile_t *)listGet(openFDList, id);
    
    if(found_fd == NULL){
        return 0;
    }

    found_fd->ref_count--;

    if(found_fd->ref_count == 0){
        removeFD(id);
    }

    return 1;
}




void removeFD(id){
    openFile_t *found_fd = (openFile_t *)listGet(openFDList, id);
    listRemove(openFDList, found_fd);
    mem_free(found_fd);
}




uint64_t addFD(void * resource, char (*read)(), char (*write)(char data), int (*close)()){
    openFile_t *new_fd = createFD(resource, read, write, close);

    if(new_fd == NULL){
        return -1;
    }

    listAdd(openFDList, new_fd);
    
    return new_fd->id; // Devolver el ID del nuevo FD
}



openFile_t * createFD(void * resource, char (*read)(void * src), char (*write)(void * dest, char data), int (*close)()) {
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

int closeFDCurrentProcess(uint64_t index) {
    processCB current = getCurrentProcess();


    openFile_t *fdToClose = current.fdTable[index];

    if(fdToClose == NULL) {
        return 0; // No FD at this index
    }
    
    uint64_t fdId = fdToClose->id;
    current.fdTable[index] = NULL; // Remove from current process's FD table


    return closeFD(fdId); // Successfully closed
}