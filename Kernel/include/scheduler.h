#ifndef _SCHEDULER_H
#define _SCHEDULER_H
#define STACK_SIZE 128
#define QUANTUM 5
#define IO_BOUND_QUANTUM 7
#define CPU_BOUND_QUANTUM 3

#include <stdint.h>

void scheduling_handler();

processQueueADT initSchedule();

uint64_t schedule(uint64_t rsp);

typedef int64_t (*program_t)(uint64_t argc, char * argv);

void fill_stack(uint64_t sp, uint64_t * initProcessWrapper, program_t entryPoint, uint64_t argc, char * argv);

void initProcessWrapper(program_t entryPoint, uint64_t argc, char * argv);


#endif