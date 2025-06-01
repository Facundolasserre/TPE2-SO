#include <scheduler.h>
#include <memoryManager.h>
#include <processQueue.h>
#include "process.h"

#define MAX_PROCESSES 100

processQueueADT processQueue = NULL;
struct Process processes[MAX_PROCESSES];
processCB currentProcess;
processQueueADT processQueue = NULL;
processQueueADT blockedQueue= NULL;



void initProcessWrapper(program_t program, uint64_t argc, char *argv[]) {
                        // RDI          ,  RSI         ,   RDX
    uint64_t returnValue = program(argc, argv);
    if(returnValue != 0){


    }
    currentProcess.state = TERMINATED; 
    exit(1); // Terminar el proceso actual
}

void initSchedule(){
    processQueue = newProcessQueue(); // Crear una nueva cola de procesos
    blockedQueue = newProcessQueue(); // Crear una nueva cola de procesos bloqueados
}


uint64_t schedule(uint64_t rsp){
    cli_asm();

    if (processQueue == NULL) {
        processQueue = initSchedule(); //inicializacion de scheduler
    }

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

void createProcessHalt(){
    createProcess(0, &halt, processQueue, 0, NULL);
}