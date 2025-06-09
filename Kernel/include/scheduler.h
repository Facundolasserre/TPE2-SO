#ifndef _SCHEDULER_H
#define _SCHEDULER_H

#define STACK_SIZE 4096
#define TOTAL_QUEUES 5
#define HIGHEST_QUEUE 3
#define AGING_THRESHOLD 100
#define ASSIGN_QUANTUM(priority) ((priority + 1) * 3)



#include <stdint.h>
#include <processQueue.h>
#include <memoryManager.h>
#include <interrupts.h>
#include <processQueue.h>
#include <fileDescriptor.h>

typedef uint64_t (*program_t)(uint64_t argc, char *argv[]);

//funciones de assembler
void halt_asm();

void cli_asm();

void sti_asm();


//funciones del scheduler
void initScheduler();

processCB create_halt_process();

uint64_t schedule(void* rsp);

processCB getNextProcess();

void initProcessWrapper(program_t entryPoint, uint64_t argc, char ** argv);


//funciones de procesos
processCB create_halt_process();

void userspaceSetFD(int *fd_ids, int fd_count);

uint64_t userspaceCreateProcessForeground(int priority, program_t program, uint64_t argc, char *argv[]);

uint64_t userspaceCreateProcess(int priority, program_t program, uint64_t argc, char *argv[]);

uint64_t kill_process(uint64_t pid);

void terminate_process(processCB process);

char * list_processes();

uint64_t block_process();

void block_process_pid(uint64_t pid);

void sendEOFForeground();

uint64_t block_current_process_to_queue(processQueueADT blockedQueue);

uint64_t block_process_to_queue(uint64_t pid, processQueueADT bloquedQueue);

uint64_t unblock_process_from_queue(processQueueADT blockedQueue);

uint64_t unblock_process(uint64_t pid);

uint64_t get_PID();

int addFileDescriptorCurrentProcess(openFile_t * fd);

void waitPid(uint64_t pid);

void yield();

void cp_halt();

uint64_t createProcess(int priority, program_t program, uint64_t argc, char *argv[], uint64_t * fdIds, uint64_t fdCount);

uint64_t create_process_state(int priority, program_t program, int state, uint64_t argc, char *argv[], uint64_t *fdIds, uint64_t fdCount, int parentPid);

int find_process_in_queue(processQueueADT queue, uint64_t pid);

int find_process_in_priority_queue(uint64_t pid);

processCB get_process_by_pid(uint64_t pid);

uint64_t setPriority(uint64_t pid, uint8_t priority);

processCB getCurrentProcess();

uint64_t createProcessForeground(int priority, program_t program, uint64_t argc, char *argv[], uint64_t * fdIds[MAX_FD], uint64_t fdCount);

uint64_t killProcessForeground();

uint64_t killProcessTerminal(char * pid);

//funciones de manejo de colas de procesos
uint8_t add_priority_queue(processCB process);

processCB find_dequeue_priority(uint64_t pid);

// funcion para llenar el stack de un proceso
void* fill_stack(uintptr_t sp, void (*initProcessWrapper)(program_t, uint64_t, char**), program_t program, uint64_t argc, char** argv);

void applyAging();

void formatProcessLine(char * line, processCB * process);



#endif