#include <scheduler.h>
#include <memoryManager.h>
#include <processQueue.h>
#include "process.h"

#define MAX_PROCESSES 100

processQueueADT processQueue = NULL;
struct Process processes[MAX_PROCESSES];
processCB currentProcess;



void initProcessWrapper(program_t entryPoint, uint64_t argc, char *argv[]) {
    // RDI          ,  RSI         ,   RDX
    uint64_t returnValue = program(argc, argv);
    if(returnValue != 0){


    }
    currentProcess.state = TERMINATED; 
    exit(1); // Terminar el proceso actual
}



uint64_t schedule(uint64_t rsp){
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
            currentProcess.usedQuantum--;

            if(currentProcess.usedQuantum <= 0) {
                currentProcess.assignedQuantum = CPU_BOUND_QUANTUM; // reiniciar quantum
            }

            currentProcess.usedQuantum = 0; // reiniciar quantum usad
            addProcessToQueue(processQueue, &currentProcess); // agregar el proceso actual a la cola
        }
    }else if(currentProcess.state == BLOCKED) {
        if(currentProcess.assignedQuantum <= IO_BOUND_QUANTUM){
            currentProcess.assignedQuantum++; // reiniciar quantum para I/O
        }
        addProcessToQueue(processQueue, &currentProcess); // agregar el proceso actual a la cola
    
    }else{
        currentProcess = *dequeueProcess(processQueue); // sacar el proceso de la cola
        currentProcess.state = RUNNING; // cambiar su estado a RUNNING
    }
    return currentProcess.rsp; // retornar el rsp del proceso actual

   
}