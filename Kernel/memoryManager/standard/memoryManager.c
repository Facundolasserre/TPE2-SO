// Este archivo implementa un administrador de memoria básico que gestiona bloques de memoria
// utilizando un enfoque de lista de punteros libres.

#include <memoryManager.h>
#include <utils.h>

// Puntero al inicio de la memoria.
void * start;

// Tamaño de la memoria y el índice actual.
int size, current;

// Lista de punteros libres.
void * free_ptrs[CHUNK_COUNT];


void mem_init(void * ptr, int s){
    start = (void *)ALIGN_POINTER(ptr, ALIGNMENT);
    
    size = s;
    current = 0;
    for(int i=0; i< CHUNK_COUNT; i++){
        free_ptrs[i]= start+ i * CHUNK_SIZE;
    }
}

void * mem_alloc(uint32_t s){
    if(current >= CHUNK_COUNT || s > CHUNK_SIZE){
        return NULL;
    }
    return free_ptrs[current++];

    //alineamos direccion del bloque actual
    void * alignedPtr = (void *) ((uintptr_t)(free_ptrs[current]) + ((ALIGNMENT) - ((uintptr_t)(free_ptrs[current]) % (ALIGNMENT))));

    //verificamos que la lineacion del bloque tenga espacio suficiente
    if((uintptr_t)(alignedPtr) + s > (uintptr_t)(free_ptrs[current]) + CHUNK_SIZE){
        current++;
        if(current >= CHUNK_COUNT){
            return NULL;
        }
        alignedPtr = (void *)ALIGN_POINTER(free_ptrs[current], ALIGNMENT);
    }

    current++;

    return alignedPtr;
}


void mem_free(void *ptr){
    if(ptr < start || ptr > start +size){
        return;
    }

    free_ptrs[--current] = ptr;
}

char * mem_state(){
    char * buffer = mem_alloc(256);
    if(buffer == NULL){
        return NULL;
    }
    int offset = 0;

    strcpy(buffer + offset, "Memory State:\n", strlen("Memory State:\n"));
    offset += strlen("Memory State:\n");
    intToStr(CHUNK_COUNT, buffer + offset);
    offset += strlen(buffer + offset);
    buffer[offset++] = '\n';

    strcpy(buffer + offset, "Chunk size: ", strlen("Chunk size: "));
    offset += strlen("Chunk size: ");
    intToStr(CHUNK_SIZE, buffer + offset);
    offset += strlen(buffer + offset);
    buffer[offset++] = '\n';

    strcpy(buffer + offset, "Current index: ", strlen("Current index: "));
    offset += strlen("Current index: ");
    intToStr(current, buffer + offset);
    offset += strlen(buffer + offset);
    buffer[offset++] = '\n';

    int freeChunks = CHUNK_COUNT - current;
    strcpy(buffer + offset, "Free chunks: ", strlen("Free chunks: "));
    offset += strlen("Free chunks: ");
    intToStr(freeChunks, buffer + offset);
    offset += strlen(buffer + offset);
    buffer[offset++] = '\n';

    buffer[offset] = '\0'; //termino la cadena
    return buffer;
}