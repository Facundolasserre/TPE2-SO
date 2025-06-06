#ifndef _SCHEDULER_H
#define _SCHEDULER_H

#define STACK_SIZE 4096
#define QUANTUM 5
#define IO_BOUND_QUANTUM 7
#define CPU_BOUND_QUANTUM 3
#define TOTAL_QUEUES 5



#include <stdint.h>
#include <processQueue.h>
#include <memoryManager.h>
#include <interrupts.h>
#include <processQueue.h>
#include <fileDescriptor.h>

typedef uint64_t (*program_t)(uint64_t argc, char *argv[]);

void halt_asm();

void cli_asm();

void sti_asm();

void init_scheduler();



processCB create_halt_process();

uint64_t schedule(void* rsp);

processCB getNextProcess();

void initProcessWrapper(program_t entryPoint, uint64_t argc, char ** argv);

uint64_t kill_process(uint64_t pid);

void list_processes();

uint64_t block_process();

uint64_t block_current_process_to_queue(processQueueADT blockedQueue);

uint64_t block_process_to_queue(uint64_t pid, processQueueADT bloquedQueue);

uint64_t unblock_process_from_queue(processQueueADT blockedQueue);

uint64_t unblock_process(uint64_t pid);

uint64_t get_PID();

int addFileDescriptorCurrentProcess(openFile_t * fd);

void waitPid(uint64_t pid);

void yield();

void cp_halt();

uint64_t createProcess(int priority, program_t program,  uint64_t argc, char *argv[], uint64_t * fdIds, uint64_t fdCount);

uint64_t create_process_state(int priority, program_t program, int state, uint64_t argc, char *argv[], uint64_t * fdIds, uint64_t fdCount);

processCB getCurrentProcess();

// Queue management
uint8_t add_priority_queue(processCB process);
processCB find_dequeue_priority(uint64_t pid);

// Stack management
void* fill_stack(uintptr_t sp, void (*initProcessWrapper)(program_t, uint64_t, char**), program_t program, uint64_t argc, char** argv);

#endif