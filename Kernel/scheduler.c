#include <scheduler.h>
#include <memoryManager.h>
#include <processQueue.h>
#include "process.h"

processQueueADT processQueue = NULL;
processCB currentProcess;
processQueueADT processQueue = NULL;
processQueueADT blockedQueue= NULL;
static int  PID = 0;

//retorna -1 por error
uint64_t createProcess(int priority, program_t program, uint64_t argc, char *argv[]) {
    return create_process_state(priority, program, READY, argc, argv);
}

//retorna -1 por error
uint64_t create_process_state(int priority, program_t program, int state, uint64_t argc, char *argv[]){
    void* stackPointer = (uint64_t)mem_alloc(STACK_SIZE);

    if(stackPointer == NULL) return -1;
    stackPointer += STACK_SIZE

    fill_stack(stackPointer,&initProcessWrapper, entry_point, argc, argv);

    Process process = {
        PID++,
        stackPointer,
        QUANTUM,
        0,
        READY};
                       // PID,    RSP,  assigned_quantum, used_quantum, state
    addProcessToQueue(queue, process);
    return process.pid;
}

void initProcessWrapper(program_t program, uint64_t argc, char *argv[]) {
                        // RDI          ,  RSI         ,   RDX
    uint64_t returnValue = program(argc, argv);
    if(returnValue != 0){


    }
    currentProcess.state = TERMINATED; 

}

void cp_halt(){
    create_process_state(0, &halt, TERMINATED, 0, NULL);
}

uint64_t get_PID(){
    return currentProcess.pid;
}

void list_processes(char *buffer){
    //8 chars para el state, 3 para el pid, 1 para un espacio y 1 para newline
    buffer = mem_alloc(10 + (get_size(processQueue) + get_size(blockedQueue)) * 13);
    // sprintf(buffer, "PID | STATE\n");
    while(has_next(processQueue)){
        processCB process = dequeueProcess(processQueue);
        // sprintf(buffer, "%s %.8s", "TODO", process.pid, process.state);
    }
    while(has_next(blockedQueue)){
        processCB process = dequeueProcess(processQueue);
        // sprintf(buffer, "%s %.8s", "TODO", process.pid, process.state);
    }
}

void kill_process(uint64_t pid){
    processCB process;
    if((process = find_pid_dequeue(processQueue, pid)).pid > 0 || (process = find_pid_dequeue(blockedQueue, pid)).pid > 0){
        process.state = TERMINATED;
    }
}

void block_process(uint64_t pid){
    processCB process;
    if((process = find_pid_dequeue(processQueue, pid)).pid > 0){
        process.state = BLOCKED;
        addProcessToQueue(blockedQueue, process);
    }
}

void unblock_process(uint64_t pid){
    processCB process;
    if((process = find_pid_dequeue(blockedQueue, pid)).pid > 0){
        process.state = READY;
        addProcessToQueue(processQueue, process);
    }
}

void yield(){
    currentProcess.state = READY;
}


void initSchedule(){
    processQueue = newProcessQueue(); // Crear una nueva cola de procesos
    blockedQueue = newProcessQueue(); // Crear una nueva cola de procesos bloqueados
    currentProcess = (processCB){0, 0, QUANTUM, 0, TERMINATED};
}


uint64_t schedule(void* rsp){
    cli_asm();

    currentProcess.rsp = rsp; // Actualizar el rsp del proceso actual

    if(currentProcess.state == RUNNING) {
        currentProcess.usedQuantum++;

        if(currentProcess.usedQuantum < currentProcess.assignedQuantum){
            return currentProcess.rsp; // si el proceso sigue en su quantum, retorna su rsp
        } else {
            currentProcess.state = READY; //si se acabo el quatum, cambia a READY
            currentProcess.usedQuantum = 0; // reiniciar quantum usado
            currentProcess.assignedQuantum--;

            if(currentProcess.usedQuantum <= 0) {
                //esto deberia ser decrementar LA PRIORIDAD creo
                currentProcess.assignedQuantum = CPU_BOUND_QUANTUM; // reiniciar quantum
            }

            
            addProcessToQueue(processQueue, &currentProcess); // agregar el proceso actual a la cola
        }
    }else if(currentProcess.state == BLOCKED) {
            if (++currentProcess.usedQuantum < currentProcess.assignedQuantum && currentProcess.assignedQuantum < IO_BOUND_QUANTUM) {
                currentProcess.assignedQuantum++; // reiniciar quantum para I/O
            } 
            addProcessToQueue(blockedQueue, &currentProcess); // agregar el proceso actual a la cola
    
     }else{ //proceso TERMINATED
        mem_free(currentProcess.rsp);
    }

    if(!hasNextProcess(processQueue)) {
        //TODO: El halt queda para siempre en la cola
        createProcessHalt(); // Si no hay más procesos, crear un proceso de parada
    }

    currentProcess = *dequeueProcess(processQueue); // Obtener el siguiente proceso de la cola
    currentProcess.state = RUNNING; // Cambiar el estado del proceso a RUNNING

    sti_asm();
    return currentProcess.rsp; // Retornar el rsp del nuevo proceso
    
}

void halt(){
    while(1){
        halt_asm();
    }
}