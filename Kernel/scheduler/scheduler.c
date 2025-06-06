#include <scheduler.h>
#include <memoryManager.h>
#include <processQueue.h>
#include <utils.h>
#include <fileDescriptor.h>
#include <openFile.h>




processCB currentProcess;
processCB haltProcess;
uint64_t PID = 0;

processQueueADT process0 = NULL;
processQueueADT process1 = NULL;
processQueueADT process2 = NULL;
processQueueADT process3 = NULL;

processQueueADT processQueue = NULL; 
processQueueADT blockedQueue = NULL; // Cola de procesos bloqueados para operaciones generales
processQueueADT blockedReadQueue = NULL; // Cola de procesos bloqueados para lectura
processQueueADT blockedSemaphoreQueue = NULL; // Cola de procesos bloqueados por semáforos

processQueueADT allBlockedQueue = NULL;


uint8_t mutexLock = 1;
int currentSemaphore = 0;
processQueueADT semQueue = NULL;



//retorna -1 por error
uint64_t createProcess(int priority, program_t program, uint64_t argc, char *argv[], uint64_t * fdIds[MAX_FD], uint64_t fdCount) {
    return create_process_state(priority, program, READY, argc, argv, fdIds, fdCount); //agregar nuevos param
}

//retorna -1 por error
uint64_t create_process_state(int priority, program_t program, int state, uint64_t argc, char *argv[], openFile_t *fdIds[MAX_FD], uint64_t fdCount) {

    void* base_pointer = mem_alloc(STACK_SIZE);

    if(base_pointer == NULL) return -1;
    

    void * stackPointer = fill_stack(base_pointer, initProcessWrapper, program, argc, argv);

   openFile_t ** fdTable = openFDTable(fdIds, fdCount);

    processCB newProcess = {
        PID++,
        stackPointer,
        QUANTUM,
        0,
        state,
        fdTable
        };

    add_priority_queue(newProcess);
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

// void cp_halt(){
//     create_process_state(0, &halt, TERMINATED, 0, NULL);
// }

uint64_t get_PID(){
    return currentProcess.pid;
}

processCB getNextProcess(){
    processCB nextProcess;
    if(hasNextProcess(process0)){
        nextProcess = dequeueProcess(process0);
    } else if(hasNextProcess(process1)){
        nextProcess = dequeueProcess(process1);
    }else if(hasNextProcess(process2)){
        nextProcess = dequeueProcess(process2);
    }else if(hasNextProcess(process3)){
        nextProcess = dequeueProcess(process3);
    }else{
        return (processCB){-1, 0, 0, 0, 0, TERMINATED, NULL};
    }
    return nextProcess;
}

processCB create_halt_process(){
    void *base_pointer = mem_alloc(STACK_SIZE);
    if(base_pointer == NULL){
         return (processCB){-1, 0, 0, 0, 0, TERMINATED, NULL};
    }
    void * stack_pointer = fill_stack(base_pointer, initProcessWrapper, &halt, 0, 0);
    processCB new_process = {
                        PID++,                 //pid
                        stack_pointer,      //rsp
                        0,                  //priority
                        QUANTUM,    //assigned_quantum
                        0,                  //used_quantum
                        HALT,             //state
                        NULL
                        };
    haltProcess = new_process;
    return new_process;
}


void list_processes(char *buffer){

    // buffer = mem_alloc();

    // if(currentProcess.pid != -1){
    //     addProcessToBuffer(currentProcess, buffer);
    // }

    // processCB process;
    // processQueueADT queues[] = {processQueue, blockedQueue, blockedReadQueue, blockedSemaphoreQueue, allBlockedQueue};

    // for( int i=0 ; i < TOTAL_QUEUES ; i++ ){
    //     while(hasNextProcess(currentProcess)){
    //         process = dequeueProcess(currentProcess);
    //         addProcessToBuffer(process, buffer);
    //         addProcessToQueue(currentProcess, process);
    //     }
    // }

    return;
}

// void addProcessToBuffer(processCB process, char * buffer){
//     char * state = getStateString(process.state);

// }

// char * getStateString(int state){
//     char * stateString;
//     switch(state){
//         case READY:
//             stateString = "READY";
//             break;
//         case RUNNING:
//             stateString = "RUNNING";
//             break;
//         case BLOCKED:
//             stateString = "BLOCKED";
//             break;
//         case TERMINATED:
//             stateString = "TERMINATED";
//             break;
//     }
// }

uint64_t kill_process(uint64_t pid){
    processCB process;
    if(currentProcess.pid == pid){
        currentProcess.state = TERMINATED;
    } else if( (process = find_dequeue_priority(pid)).pid > 0 || (process = find_pid_dequeue(blockedQueue, pid)).pid > 0 ){
        mem_free(process.rsp);
    } else {
        return -1;
    }
}



uint64_t block_process(uint64_t){
    currentProcess.state = BLOCKED;
    __asm__ ("int $0x20"); // timertick para llamar a schedule de nuevo
}

uint64_t block_current_process_to_queue(processQueueADT blockedQ){
    currentProcess.state = BLOCKED;
    addProcessToQueue(blockedQueue, currentProcess);
    __asm__ ("int $0x20"); // timertick para llamar a schedule de nuevo
}

uint64_t block_process_to_queue(uint64_t pid, processQueueADT destination){
    processCB process;
   if(currentProcess.pid == pid){
        block_current_process_to_queue(destination);
   }else if( (process = find_dequeue_priority(pid)).pid > 0){
        process.state = BLOCKED;
        addProcessToQueue(destination, process);
        addProcessToQueue(allBlockedQueue, process);
    } else {
        return -1;
    }
    return 0;
}


uint64_t unblock_process(uint64_t pid){
    processCB process;
    if((process = find_pid_dequeue(allBlockedQueue, pid)).pid > 0){
        process.state = READY;
        add_priority_queue(process);
    } else {
        return -1;
    }
}

void yield(){
    currentProcess.state = READY;
}

void initScheduler(){

    currentSemaphore = 0;

    process0 = newProcessQueue();
    process1 = newProcessQueue();
    process2 = newProcessQueue();
    process3 = newProcessQueue();

    
    allBlockedQueue = newProcessQueue();
    
   

    currentProcess = (processCB){0, 0, 0, 0, 0, TERMINATED, NULL};

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

            
            add_priority_queue(currentProcess); // agregar el proceso actual a la cola
        }
    }else if(currentProcess.state == BLOCKED) {
            if (++currentProcess.usedQuantum < currentProcess.assignedQuantum && currentProcess.assignedQuantum < IO_BOUND_QUANTUM) {
                currentProcess.assignedQuantum++; // reiniciar quantum para I/O
            } 
            addProcessToQueue(allBlockedQueue, currentProcess); // agregar el proceso actual a la cola
    
     }else if (currentProcess.state == READY){ //proceso TERMINATED
        add_priority_queue(currentProcess);
    } else if (currentProcess.state == TERMINATED){
        for(int i=0; i< MAX_FD; i++){
            if(currentProcess.fdTable[i] != NULL){
                closeFD(currentProcess.fdTable[i]->id); // cerrar el fd del proceso actual
            }

        }
        mem_free(currentProcess.rsp); // liberar la memoria del stack del proceso actual
    }

    
    currentProcess = getNextProcess();
    if(currentProcess.state != READY){
        currentProcess = haltProcess;
    }else{
       currentProcess.state = RUNNING; 
    }
    
    return currentProcess.rsp;
}


// Desbloquea el primer proceso esperando en la cola recibida por parametro
uint64_t unblock_process_from_queue(processQueueADT source){
    processCB temp = dequeueProcess(source);

    if(temp.pid < 0){
        return -1;
    }

    processCB process = find_pid_dequeue(allBlockedQueue, temp.pid);

    if(process.pid < 0){
        return -1;
    }

    process.state = READY;
    add_priority_queue(process);
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

    return (processCB){-1, 0, 0, 0, 0, TERMINATED, NULL};
}

// int closeFDCurrentProcess(uint64_t index){
//     uint64_t fdId = currentProcess.fdTable[index]->id;
//     currentProcess.fdTable[index] = NULL;
//     return closeFD(fdId);
// }

// //Agrega un FD al proceso actual
// int addFileDescriptorCurrentProcess(uint64_t fd_id){
//     //ARREGLAR: compare
//     for(int i = 0; i < MAX_FD; i++){
//         if(compare_file_descriptors(currentProcess.fdTable[i], fd_id) == 0){
//             return i;
//         }
//     }

//     for(int i = 0; i < MAX_FD; i++){
//         if(currentProcess.fdTable[i] == NULL){
//             currentProcess.fdTable[i] = fd_id;
//             return i;
//         }
//     }

//     return -1;
// }

// //Elimina un FD de la tabla de FD del proceso actual
// int removeFileDescriptorCurrentProcess(openFile_t *fd){
//     //ARREGLAR: compare
//     for(int i = 0; i < MAX_FD; i++){
//         if(compare_file_descriptors(currentProcess.fdTable[i], fd) == 0){
//             currentProcess.fdTable[i] = NULL;
//             return 1;
//         }
//     }
//     return 0;
// }

processCB getCurrentProcess(){
    return currentProcess;
}
