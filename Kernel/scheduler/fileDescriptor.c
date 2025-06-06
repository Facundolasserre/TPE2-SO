
#include <fileDescriptor.h>
#include <scheduler.h>
#include <stdlib.h>
#include <list.h>
#include <memoryManager.h>
#include <keyboard.h>




List *openFDList;
uint64_t current_id = 0;
openFile_t *STDIN;
openFile_t *STDOUT;



int compare_file_descriptors(void *this, void *other_fd) {
   
    return 0;
}

void init_file_descriptors(){
    openFDList = list_init(compare_file_descriptors);
    STDIN = create_fd(readKeyboard, NULL, NULL);
    

    if (openFDList == NULL) {
        return -1;
    }

    return 0;
}

openFile_t *get_stdin(){
    return STDIN;
}

openFile_t *get_stdout(){
    return STDOUT;
}

//@returns el indice del FD en la tabla de FD del proceso actual
int fd_manager_open(uint64_t id){
    openFile_t *found_fd = (openFile_t *)listGet(openFDList, id);
    
    if(found_fd == NULL){
        return -1;
    }

    found_fd->ref_count++;

    return addFileDescriptorCurrentProcess(found_fd);
}


int fd_manager_close(uint64_t id){
    openFile_t *found_fd = (openFile_t *)listGet(openFDList, id);
    
    if(found_fd == NULL){
        return -1;
    }

    found_fd->ref_count--;

    if(found_fd->ref_count == 0){
        remove_fd(id);
    }

    removeFileDescriptorCurrentProcess(found_fd);

    return 0;
}

void remove_fd(id){
    openFile_t *found_fd = (openFile_t *)listGet(openFDList, id);
    list_remove(openFDList, found_fd);
    mem_free(found_fd);
}

int add_fd(char (*read)(), char (*write)(char data), int (*close)()){
    openFile_t *new_fd = create_fd(read, write, close);

    if(new_fd == -1){
        return -1;
    }

    list_add(openFDList, new_fd);
    return 1;
}

openFile_t * create_fd(char (*read)(), char (*write)(char data), int (*close)()) {
    openFile_t * new_fd = (openFile_t *)mem_alloc(sizeof(openFile_t));
    
    if(new_fd == NULL){
        return -1;
    }

    new_fd->id = current_id++;
    new_fd->read = read;
    new_fd->write = write;
    new_fd->close = close;
    new_fd->ref_count = 1;

    return new_fd;
}