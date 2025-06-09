#ifndef _TEST_UTIL_H
#define _TEST_UTIL_H

//test de la catedra
#include <stdint.h>

uint32_t GetUint();

uint8_t memcheck(void *start, uint8_t value, uint32_t size);

void bussy_wait(uint64_t n);
void endless_loop();
void endless_loop_print(uint64_t wait);

void slowInc(int64_t * p, int64_t inc);

uint64_t my_process_inc(uint64_t argc, char *argv[]);

uint64_t test_sync(uint64_t argc, char *argv[]);


#endif