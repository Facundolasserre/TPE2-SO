#ifndef _MEM_MANAGER_H_
#define _MEM_MANAGER_H_

#include <stddef.h>
#include <stdint.h>


#define CHUNK_SIZE 10
#define CHUNK_COUNT 4096
#define MEMORY_START 0x300000


void * mem_init(int s);

void * mem_alloc(uint32_t s);

void mem_free(void *p);


#endif 