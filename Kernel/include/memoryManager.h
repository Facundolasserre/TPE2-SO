// Este archivo contiene las definiciones y declaraciones para el administrador de memoria.
// Proporciona funciones para inicializar, asignar y liberar memoria.

#ifndef _MEM_MANAGER_H_
#define _MEM_MANAGER_H_

#include <stddef.h>
#include <stdint.h>


#define CHUNK_SIZE 4096 //tamaño de cada bloque de memoria
#define CHUNK_COUNT 32768 //cantidad de bloques
#define MEMORY_START 0xF00000 //donde comienza la memoria

#define ALIGNMENT 8 //word align
#define ALIGN_POINTER(ptr, alignment) ((uintptr_t)(ptr) + ((alignment) - ((uintptr_t)(ptr) % (alignment))))

// Inicializa el administrador de memoria con un puntero y tamaño dados.
void mem_init(void * ptr, uint64_t s);

// Asigna un bloque de memoria de un tamaño dado.
void * mem_alloc(uint32_t s);

// Libera un bloque de memoria dado un puntero.
void mem_free(void *p);

char * mem_state();

#endif 