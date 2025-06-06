// Este archivo implementa un administrador de memoria básico que gestiona bloques de memoria
// utilizando un enfoque de lista de punteros libres.

#include <memoryManager.h>

// Puntero al inicio de la memoria.
void * start;

// Tamaño de la memoria y el índice actual.
int size, current;

// Lista de punteros libres.
void * free_ptrs[CHUNK_COUNT];


void mem_init(void * ptr, int s){
    start = ALIGN_POINTER(ptr, ALIGNMENT);
    
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