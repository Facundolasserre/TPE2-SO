#include "process.h"

static uint64_t next_pid = 1;
static PCB *process_list[MAX_PROCESSES];
static int process_count = 0;

void init_process_list(void) {
    process_count = 0;
    for (int i = 0; i < MAX_PROCESSES; i++) {
        process_list[i] = NULL;
    }
}

void add_process(PCB *process) {
    if (process_count < MAX_PROCESSES) {
        process_list[process_count++] = process;
    }
}

int get_process_list(PCB *buffer, int max) {
    int count = 0;
    for (int i = 0; i < process_count && count < max; i++) {
        if (process_list[i] != NULL) {
            buffer[count++] = *process_list[i];
        }
    }
    return count;
}

PCB *create_process(int priority, void *entry_point) {
    PCB *process = (PCB *)mem_alloc(sizeof(PCB));
    if (!process) return NULL;

    process->pid = next_pid++;
    process->state = READY;
    process->priority = priority;
    process->stack = mem_alloc(4096);
    if (!process->stack) {
        mem_free(process);
        return NULL;
    }

    process->rsp = (uint64_t)process->stack + 4096;
    process->rip = (uint64_t)entry_point;

    add_process(process); // Agregar a la lista
    return process;
}

void free_process(PCB *process) {
    if (process) {
        for (int i = 0; i < process_count; i++) {
            if (process_list[i] == process) {
                process_list[i] = NULL;
                break;
            }
        }
        mem_free(process->stack);
        mem_free(process);
    }
}