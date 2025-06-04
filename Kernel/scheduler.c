#include <scheduler.h>
#include <memoryManager.h>
#include <processQueue.h>
#include <utils.h>



processCB currentProcess;
uint64_t PID = 0;

processQueueADT processQueue = NULL; 
processQueueADT blockedQueue = NULL; // Cola de procesos bloqueados para operaciones generales
processQueueADT blockedReadQueue = NULL; // Cola de procesos bloqueados para lectura
processQueueADT blockedSemaphoreQueue = NULL; // Cola de procesos bloqueados por semáforos

processQueueADT allBlockedQueue = NULL;
processQueueADT blockedReadQueue = NULL;

uint8_t mutexLock = 1;
int currentSemaphore = 0;
processQueueADT semQueue = NULL;



//retorna -1 por error
uint64_t createProcess(int priority, program_t program, uint64_t argc, char *argv[]) {
    return create_process_state(priority, program, READY, argc, argv);
}

//retorna -1 por error
uint64_t create_process_state(int priority, program_t program, int state, uint64_t argc, char *argv[]){
    void* base_pointer = mem_alloc(STACK_SIZE);

    if(base_pointer == NULL) return -1;
    //base_pointer += STACK_SIZE

    void * stackPointer = fill_stack(base_pointer, initProcessWrapper, program, argc, argv);

    processCB newProcess = {
        PID++,
        stackPointer,
        QUANTUM,
        0,
        READY};
                       // PID,    RSP,  assigned_quantum, used_quantum, state
    addProcessToQueue(processQueue, newProcess); //TO DO: CHEQUEAR LO DEL AMPERSAND NO ESTOY SEGURO SI VA O NO
    return newProcess.pid;
}

void initProcessWrapper(program_t program, uint64_t argc, char *argv[]) {
                        // RDI          ,  RSI         ,   RDX
    uint64_t returnValue = program(argc, argv);
    if(returnValue != 0){


    }
    currentProcess.state = TERMINATED; 

    __asm__ ("int $0x20"); //rimertick

}

void halt(){
    while(1){
        halt_asm();
    }
}

void cp_halt(){
    create_process_state(0, &halt, TERMINATED, 0, NULL);
}

uint64_t get_PID(){
    return currentProcess.pid;
}

processCB getNextProcess(){
    processCB nextProcess;
    if(hasNextProcess(processQueue)){
        nextProcess = dequeueProcess(processQueue);
    } else if(hasNextProcess(blockedQueue)){
        nextProcess = dequeueProcess(blockedQueue);
    }else if(hasNextProcess(blockedReadQueue)){
        nextProcess = dequeueProcess(blockedReadQueue);
    }else if(hasNextProcess(blockedSemaphoreQueue)){
        nextProcess = dequeueProcess(blockedSemaphoreQueue);
    }else{
        return (processCB){-1, 0, 0, 0, 0, TERMINATED};
    }
    return nextProcess;
}

void list_processes(char *buffer){
    // //8 chars para el state, 3 para el pid, 1 para un espacio y 1 para newline
    // buffer = mem_alloc(10 + (get_size(processQueue) + get_size(blockedQueue)) * 13);
    // // sprintf(buffer, "PID | STATE\n");
    // while(hasNextProcess(processQueue)){
    //     processCB process = dequeueProcess(processQueue);
    //     // sprintf(buffer, "%s %.8s", "TODO", process.pid, process.state);
    // }
    // while(hasNextProcess(blockedQueue)){
    //     processCB process = dequeueProcess(processQueue);
    //     // sprintf(buffer, "%s %.8s", "TODO", process.pid, process.state);
    // }
}

uint64_t kill_process(uint64_t pid){
    processCB process;
    if(currentProcess.pid == pid){
        currentProcess.state = TERMINATED;
    } else if( (process = find_pid_dequeue(processQueue, pid)).pid > 0 || (process = find_pid_dequeue(blockedQueue, pid)).pid > 0 ){
        mem_free(process.rsp);
    } else {
        return -1;
    }
}



uint64_t block_process(uint64_t){
    return block_process_to_queue(currentProcess.pid, blockedQueue);
}

uint64_t block_current_process_to_queue(processQueueADT blockedQueue){
    return block_process_to_queue(currentProcess.pid, blockedQueue);
}

uint64_t block_process_to_queue(uint64_t pid, processQueueADT blockedQueue){
    processCB process;
   if(currentProcess.pid == pid){
        currentProcess.state = BLOCKED;
   }else if( (process = find_pid_dequeue(processQueue, pid)).pid > 0){
        process.state = BLOCKED;
        addProcessToQueue(blockedQueue, process);
        addProcessToQueue(allBlockedQueue, process);
    } else {
        return -1;
    }
}


uint64_t unblock_process(uint64_t pid){
    processCB process;
    if((process = find_pid_dequeue(blockedQueue, pid)).pid > 0){
        process.state = READY;
        addProcessToQueue(processQueue, process);
    } else {
        return -1;
    }
}

void yield(){
    currentProcess.state = READY;
}

void initScheduler(){

    currentSemaphore = 0;

    processQueue = newProcessQueue(); // Crear una nueva cola de procesos
    blockedQueue = newProcessQueue(); // Crear una nueva cola de procesos bloqueados
    blockedReadQueue = newProcessQueue();
    blockedSemaphoreQueue = newProcessQueue();
    allBlockedQueue = newProcessQueue();
    currentProcess = (processCB){0, 0, QUANTUM, 0, TERMINATED};

}


uint64_t schedule(void* rsp){

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
                //esto deberia ser decrementar LA PRIORIDAD
                currentProcess.assignedQuantum = CPU_BOUND_QUANTUM; // reiniciar quantum
            }

            
            addProcessToQueue(processQueue, currentProcess); // agregar el proceso actual a la cola
        }
    }else if(currentProcess.state == BLOCKED) {
            if (++currentProcess.usedQuantum < currentProcess.assignedQuantum && currentProcess.assignedQuantum < IO_BOUND_QUANTUM) {
                currentProcess.assignedQuantum++; // reiniciar quantum para I/O
            } 
            addProcessToQueue(blockedQueue, currentProcess); // agregar el proceso actual a la cola
    
     }else{ //proceso TERMINATED
        mem_free(currentProcess.rsp);
    }

    if(!hasNextProcess(processQueue)) {
        //TODO: El halt queda para siempre en la cola
        cp_halt(); // Si no hay más procesos, crear un proceso de parada
    }

    currentProcess = dequeueProcess(processQueue); // Obtener el siguiente proceso de la cola
    currentProcess.state = RUNNING; // Cambiar el estado del proceso a RUNNING

    return currentProcess.rsp; // Retornar el rsp del nuevo proceso
    
}


// Desbloquea el primer proceso esperando en la cola recibida por parametro
uint64_t unblock_process_from_queue(processQueueADT blockedQueue){
    processCB process = dequeueProcess(blockedQueue);
    process.state = READY;
    addProcessToQueue(processQueue, process);
}

uint8_t add_priority_queue(processCB process){
    switch(process.priority){
        case(0):
            addProcessToQueue(processQueue, process);
            break;
        case(1):
            addProcessToQueue(blockedQueue, process);
            break;
        case(2):
            addProcessToQueue(blockedReadQueue, process);
            break;
        case(3):
            addProcessToQueue(blockedSemaphoreQueue, process);
            break;
        default:
            return -1;
    }
}

processCB find_dequeue_priority(uint64_t pid){
    processCB process;
    process = find_pid_dequeue(processQueue, pid);
    if(process.pid > 0) return process;

    process = find_pid_dequeue(blockedQueue, pid);
    if(process.pid > 0) return process;

    process = find_pid_dequeue(blockedReadQueue, pid);
    if(process.pid > 0) return process;

    process = find_pid_dequeue(blockedSemaphoreQueue, pid);
    if(process.pid > 0) return process;

    return (processCB){-1, 0, 0, 0, 0, TERMINATED};
}
