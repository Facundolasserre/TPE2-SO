#ifndef _MEM_MANAGER_H_
#define _MEM_MANAGER_H_

#include <stddef.h>
#include <stdint.h>


#define CHUNK_SIZE 4096
#define CHUNK_COUNT 32768
#define MEMORY_START 0x300000

#define ALIGNMENT 8 //word align
#define ALIGN_POINTER(ptr, alignment) \
    ((uintptr_t)(ptr) + ((alignment) - ((uintptr_t)(ptr) % (alignment))))

void * mem_init(int s);

void * mem_alloc(uint32_t s);

void mem_free(void *p);

#endif 