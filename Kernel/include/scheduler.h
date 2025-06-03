#ifndef _SCHEDULER_H
#define _SCHEDULER_H

#define STACK_SIZE 4096
#define QUANTUM 5
#define IO_BOUND_QUANTUM 7
#define CPU_BOUND_QUANTUM 3

#include <stdint.h>
#include <processQueue.h>

typedef uint64_t (*program_t)(uint64_t argc, char *argv[]);

void halt_asm();

void cli_asm();

void sti_asm();

void scheduling_handler();

void init_scheduler();

uint64_t create_process(int priority, program_t program, uint64_t argc, char *argv[]);

uint64_t create_process_state(int priority, program_t program, int state, uint64_t argc, char *argv[]);

void initSchedule();

uint64_t schedule(void* rsp);

void *fill_stack(uintptr_t sp, void (* initProcessWrapper)(program_t, uint64_t, char**), program_t entryPoint, uint64_t argc, char ** argv);

void initProcessWrapper(program_t entryPoint, uint64_t argc, char ** argv);

uint64_t kill_process(uint64_t pid);

void list_processes(char * buffer);

uint64_t block_process(uint64_t pid);

uint64_t unblock_process(uint64_t pid);

uint64_t get_PID();

void yield();

void cp_halt();

uint64_t createProcess(int priority, program_t program,  uint64_t argc, char *argv[]);

uint64_t create_process_state(int priority, program_t program, int state, uint64_t argc, char *argv[]);

#endif