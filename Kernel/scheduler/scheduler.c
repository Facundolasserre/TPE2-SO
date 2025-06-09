// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
#include <scheduler.h>
#include <memoryManager.h>
#include <processQueue.h>
#include <utils.h>
#include <fileDescriptor.h>
#include <openFile.h>
#include <list.h>
#include <videoDriver.h>




processCB currentProcess;
processCB haltProcess;
uint64_t PID = 0;
int foreGroundPID = -2; 
uint64_t userspaceProcessCreationFDIds[MAX_FD] = {0, 1};
uint64_t userspaceProcessCreationFDCount = 0; // Cantidad de FDI de los procesos creados por el usuario

uint64_t agingCounter = 0; // Contador para el envejecimiento de procesos

processQueueADT priority0 = NULL;
processQueueADT priority1 = NULL;
processQueueADT priority2 = NULL;
processQueueADT priority3 = NULL;

//processQueueADT processQueue = NULL; 
//processQueueADT blockedQueue = NULL; // Cola de procesos bloqueados para operaciones generales
processQueueADT blockedReadQueue = NULL; // Cola de procesos bloqueados para lectura
processQueueADT blockedSemaphoreQueue = NULL; // Cola de procesos bloqueados por semáforos
processQueueADT allBlockedQueue = NULL;


uint8_t mutexLock = 1;
int currentSemaphore = 0;


void initScheduler(){

    currentSemaphore = 0;

    priority0 = newProcessQueue();
    priority1 = newProcessQueue();
    priority2 = newProcessQueue();
    priority3 = newProcessQueue();

    
    allBlockedQueue = newProcessQueue();
    
   

    currentProcess = returnNullProcess();
    haltProcess = create_halt_process();

}

//retorna -1 por error
uint64_t createProcess(int priority, program_t program, uint64_t argc, char *argv[], uint64_t * fdIds, uint64_t fdCount) {
    return create_process_state(priority, program, READY, argc, argv, fdIds, fdCount, 0); //agregar nuevos param
}

//retorna -1 por error
uint64_t create_process_state(int priority, program_t program, int state, uint64_t argc, char *argv[], uint64_t *fdIds, uint64_t fdCount, int parentPid) {

    void* base_pointer = mem_alloc(STACK_SIZE);

    if(base_pointer == NULL){
        return -1;
    }

    void * stackPointer = fill_stack((uintptr_t)base_pointer, initProcessWrapper, program, argc, argv);

    processQueueADT childrenList = newProcessQueue();

   openFile_t ** fdTable = openFDTable(fdIds, fdCount);

   int assigned_quantum = ASSIGN_QUANTUM(priority);

    processCB newProcess = {
                        PID++,       //pid
                        base_pointer,       //base_sp
                        stackPointer,      //rsp
                        priority,           //priority
                        assigned_quantum,   //assigned_quantum
                        0,                  //used_quantum
                        state,              //state
                        fdTable,           //fd_table
                        childrenList,      //children_list
                        parentPid               //p_pid
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
    if(hasNextProcess(priority0)){
        nextProcess = dequeueProcess(priority0);
    } else if(hasNextProcess(priority1)){
        nextProcess = dequeueProcess(priority1);
    }else if(hasNextProcess(priority2)){
        nextProcess = dequeueProcess(priority2);
    }else if(hasNextProcess(priority3)){
        nextProcess = dequeueProcess(priority3);
    }else{
        return returnNullProcess();
    }
    return nextProcess;
}

processCB create_halt_process(){
    void *base_pointer = mem_alloc(STACK_SIZE);
    if(base_pointer == NULL){
         return returnNullProcess();
    }
    //Y ESTO
    void * stack_pointer = fill_stack((uintptr_t)base_pointer, initProcessWrapper, (program_t)halt, 0, NULL);
    
    processCB new_process = {
                        PID++,       //pid
                        base_pointer,       //base_sp
                        stack_pointer,      //rsp
                        0,                  //priority
                        0,                  //assigned_quantum
                        0,                  //used_quantum
                        HALT,               //state
                        NULL,               //fd_table
                        NULL,               //children_list
                        0                   //p_pid
                        };
      
    return new_process;
}


char * list_processes(){

    //tamaño aprox de cada linea seria: (pid + priority + state + separadores y \n)

    const int line_size = 100; //tamaño de cada linea
    const int header_size = 100; //tamaño aproximado del encabezado

    char * header = "PID   PRIORITY  STATE  BASE POINTER PPID\n";

    //calculo el tamaño necesario para almacenar todos los procesos
    int totalProcesses = 1; //1 para incluir el proceso actual
    processCB process;
    processQueueADT queues[] = {priority0, priority1, priority2, priority3, allBlockedQueue};

    for(int i = 0; i < TOTAL_QUEUES; i++){
        totalProcesses += getQueueSize(queues[i]);
    }

    //asigno memoria para el buffer
    char * buffer = mem_alloc(header_size + (totalProcesses * line_size));
    int offset = 0;

    strcpy(buffer, header, header_size); //copio el encabezado al buffer
    offset += strlen(header); //actualizo el offset

    //agrego el proceso actual si esta definido
    if(currentProcess.pid != -1){
        char line[line_size];
        formatProcessLine(line, &currentProcess);
        strcpy(buffer + offset, line, line_size);
        offset += strlen(line);
    }


    //proceso cada cola de procesos
    for(int i=0 ; i<TOTAL_QUEUES ; i++){
        processQueueADT current = queues[i];
        int size = getQueueSize(current);

        //agrego cada proceso a la cola del buffer
        for(int j=0 ; j<size ; j++){
            process = dequeueProcess(current);
            //formateo cada proceso en una linea
            char line[line_size];
            formatProcessLine(line, &process);

            //copio la linea al buffer final
            strcpy(buffer + offset, line, line_size);
            offset += strlen(line);

            //volver a agregar el proceso a la cola original
            addProcessToQueue(current, process);
        }
    }

    return buffer;
}

void formatProcessLine(char *line, processCB * process){
    char pid_str[10];
    char priority_str[5];
    char base_pointer_str[10];
    char ppid_str[10];
    char * state_str;

    //convierto los enteros a cadena
    intToStr(process->pid, pid_str);
    intToStr(process->parentPid, ppid_str);
    intToStr(process->priority, priority_str);
    intToStr((int)(uintptr_t)process->base_pointer, base_pointer_str);

    //determio la cadena de estado
    switch(process->state){
        case READY: state_str = "READY"; break;
        case RUNNING: state_str = "RUNNING"; break;
        case BLOCKED: state_str = "BLOCKED"; break;
        case TERMINATED: state_str = "TERMINATED"; break;
        case HALT: state_str = "HALT"; break;
    }

    //construyo la linea en el formato "PID   PRIORITY   STATE\n"
    int offset = 0;

    //copio el pid en la linea
    strcpy(line + offset, pid_str, strlen(pid_str));
    offset += strlen(pid_str);
    for(int i=0; i<3; i++){
        line[offset++] = ' '; // espacio entre pid y priority
    }

    //copio el priority en la linea
    strcpy(line + offset, priority_str, strlen(priority_str));
    offset += strlen(priority_str);
    for(int i =0; i<8; i++){
        line[offset++] = ' '; // espacio entre priority y base_pointer
    }
    line[offset++] = ' '; // espacio entre priority y state

    //copio el state en la linea
    strcpy(line + offset, state_str, strlen(state_str));
    offset += strlen(state_str);
    for(int i=0; i<2; i++){
        line[offset++] = ' '; // espacio entre state y base_pointer
    }

    //copio el base_pointer en la linea
    strcpy(line + offset, base_pointer_str, strlen(base_pointer_str));
    offset += strlen(base_pointer_str);
    line[offset++] = ' '; // espacio al final del base_pointer

    strcpy(line + offset, ppid_str, strlen(ppid_str));
    offset += strlen(ppid_str);
    line[offset++] = ' ';

    //agrego un salto de linea al final
    line[offset++] = '\n';
    line[offset] = '\0'; //termino la cadena
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
        __asm__ ("int $0x20");                 // TimerTick para llamar a schedule de nuevo
        return 1;
    } else if( (process = find_dequeue_priority(pid)).pid > 0 || (process = findPidDequeue(allBlockedQueue, pid)).pid > 0 ){
        terminate_process(process);
        return 1;
    } else {
        return 0;
    }
    return 0;
}

void terminate_process(processCB process){
    if(process.pid == foreGroundPID && process.fdTable != NULL){  
        process.fdTable[1]->write(process.fdTable[1]->resource, -1);
        foreGroundPID  = -1;
    }
      
    if(process.fdTable != NULL){
        for(int i = 0; i < MAX_FD; i++){
            if(process.fdTable[i] != NULL){
                closeFD(process.fdTable[i]->id);
            }
        }
    }
            
    processCB parent = get_process_by_pid(process.parentPid);

    // Establezco al padre de mis hijos como MI padre.
    if(parent.pid >= 0){
        size_t size = getQueueSize(process.childrenList);
        for(int i = 0; i < size; i++){
            processCB aux = dequeueProcess(process.childrenList);
            
            processCB child = find_dequeue_priority(aux.pid);

            if (child.pid > 0){
                child.parentPid = process.parentPid;
                add_priority_queue(child);
            } else {
                child = findPidDequeue(allBlockedQueue, aux.pid);
                if (child.pid > 0){
                    child.parentPid = process.parentPid;
                    addProcessToQueue(allBlockedQueue, child);
                }
            }

            if (parent.pid != 0){
                addProcessToQueue(parent.childrenList, aux);
            }
        }
    }

    // Me quito de la lista de hijos de mi padre, desbloqueo a mi padre.
    if(parent.pid > 0){
        findPidDequeue(parent.childrenList, process.pid);
        unblock_process(process.parentPid);
    }
    
    mem_free(process.base_pointer);
    freeQueue(process.childrenList);
}



void block_process_pid(uint64_t pid){
    if(currentProcess.pid == pid){
        return;
    }
    processCB process = find_dequeue_priority(pid);

    if(process.pid < 0){
        return; // No se encontro el proceso
    }
    process.state = BLOCKED;
    addProcessToQueue(allBlockedQueue, process);

    return;
}


uint64_t block_process(){
    currentProcess.state = BLOCKED;
    __asm__ ("int $0x20"); // timertick para llamar a schedule de nuevo
    return 0;
}

uint64_t block_current_process_to_queue(processQueueADT blockedQ){
    currentProcess.state = BLOCKED;
    addProcessToQueue(blockedQ, currentProcess);
    __asm__ ("int $0x20"); // timertick para llamar a schedule de nuevo
    return 0;
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
    if((process = findPidDequeue(allBlockedQueue, pid)).pid > 0){
        process.state = READY;
        add_priority_queue(process);
    } else {
        return -1;
    }
    return 0;
}

void yield(){
    currentProcess.state = READY;
}




uint64_t schedule(void * rspRunning){

    currentProcess.rsp = rspRunning;

    if(++agingCounter >= AGING_THRESHOLD){
        agingCounter = 0;
        applyAging();
    }

    switch(currentProcess.state){
        case RUNNING:
            currentProcess.usedQuantum++;

            if(currentProcess.usedQuantum < currentProcess.assignedQuantum){
                return (uint64_t)currentProcess.rsp;
            } else {
                currentProcess.state = READY;
                currentProcess.priority = (currentProcess.priority + 1) > HIGHEST_QUEUE ? HIGHEST_QUEUE : currentProcess.priority + 1;
                currentProcess.assignedQuantum = ASSIGN_QUANTUM(currentProcess.priority);
                currentProcess.usedQuantum = 0;
                add_priority_queue(currentProcess);
            }
            break;

        case BLOCKED:
            // Si el proceso se bloquea antes de usar sus quantums, lo paso a una cola de mejor prioridad
            if(++currentProcess.usedQuantum < currentProcess.assignedQuantum){
                currentProcess.priority = (currentProcess.priority - 1) < 0 ? 0 : currentProcess.priority - 1;
                currentProcess.assignedQuantum = ASSIGN_QUANTUM(currentProcess.priority);
            }

            currentProcess.usedQuantum = 0;

            addProcessToQueue(allBlockedQueue, currentProcess);
            break;

        case READY:
            add_priority_queue(currentProcess);
            break;

        case HALT:
            haltProcess.rsp = rspRunning;
            break;

        case TERMINATED:
            terminate_process(currentProcess);
            break;

        default:            // se estaba ejecutando el proceso halt
            break;
    }

    processCB next_process = getNextProcess();
    
    if(next_process.state == READY){
        currentProcess = next_process;
        currentProcess.state = RUNNING;
    } else {
        currentProcess = haltProcess;
    }

    return (uint64_t)currentProcess.rsp;
}



uint64_t userspaceCreateProcessForeground(int priority, program_t program, uint64_t argc, char *argv[]) {
    foreGroundPID = create_process_state(priority, program, READY, argc, argv, userspaceProcessCreationFDIds, userspaceProcessCreationFDCount, currentProcess.pid);
    addProcessToQueue(currentProcess.childrenList, get_process_by_pid(foreGroundPID));
    return foreGroundPID;
}

uint64_t userspaceCreateProcess(int priority, program_t program, uint64_t argc, char *argv[]){
    uint64_t pid = create_process_state(priority, program, READY, argc, argv, userspaceProcessCreationFDIds, userspaceProcessCreationFDCount, currentProcess.pid);
    addProcessToQueue(currentProcess.childrenList, get_process_by_pid(pid));
    return pid;
}

uint64_t killProcessForeground(){
    if(foreGroundPID < 0){
        return -1;
    }
    int aux = foreGroundPID;
    kill_process(foreGroundPID);
    foreGroundPID = -2;
    return aux; // Retornar el pid del proceso que se mató
   
}

void sendEOFForeground(){
    if(foreGroundPID < 0){
        return;
    }
    processCB process;

    if(currentProcess.pid == foreGroundPID){
        process = currentProcess;
    } else if( (process = get_process_by_pid(foreGroundPID)).pid < 0){
        return; // No se encontró el proceso
    }

    process.fdTable[1]->write(process.fdTable[1]->resource, -1);
    process.fdTable[0]->write(process.fdTable[0]->resource, -1); 
}


// Desbloquea el primer proceso esperando en la cola recibida por parametro
uint64_t unblock_process_from_queue(processQueueADT source){
    processCB temp = dequeueProcess(source);

    if(temp.pid < 0){
        return -1;
    }

    processCB process = findPidDequeue(allBlockedQueue, temp.pid);

    if(process.pid < 0){
        return -1;
    }

    process.state = READY;
    add_priority_queue(process);
    return 0;
}

uint8_t add_priority_queue(processCB process){
    process.assignedQuantum = ASSIGN_QUANTUM(process.priority);
    switch(process.priority){
        case(0):
            addProcessToQueue(priority0, process);
            break;
        case(1):
            addProcessToQueue(priority1, process);
            break;
        case(2):
            addProcessToQueue(priority2, process);
            break;
        case(3):
            addProcessToQueue(priority3, process);
            break;
        default:
            return -1;
    }
    return 0;
}

uint64_t setPriority(uint64_t pid, uint8_t priority){
    priority = priority % (HIGHEST_QUEUE+1);
    processCB process;
    if(currentProcess.pid == pid){
        currentProcess.priority = priority;
        currentProcess.usedQuantum = 0;
        currentProcess.assignedQuantum = ASSIGN_QUANTUM(priority);
    } else if ((process = find_dequeue_priority(pid)).pid > 0){
        process.usedQuantum = 0;
        process.priority = priority;
        add_priority_queue(process);
    } else if( (process = findPidDequeue(allBlockedQueue, pid)).pid > 0){
        process.priority = priority;
        process.usedQuantum = 0;
        process.assignedQuantum = ASSIGN_QUANTUM(priority);
        addProcessToQueue(allBlockedQueue, process);
    } else {
        return -1;
    }
    return 0;
}

processCB find_dequeue_priority(uint64_t pid){
    processCB process;
    process = findPidDequeue(priority0, pid);
    if(process.pid > 0) return process;

    process = findPidDequeue(priority1, pid);
    if(process.pid > 0) return process;

    process = findPidDequeue(priority2, pid);
    if(process.pid > 0) return process;

    process = findPidDequeue(priority3, pid);
    if(process.pid > 0) return process;

    return returnNullProcess();
}

int find_process_in_priority_queue(uint64_t pid){
    processQueueADT queues[] = {priority0, priority1, priority2, priority3, allBlockedQueue};
    for(int i=0 ; i<4 ; i++){
        if(find_process_in_queue(queues[i], pid)){
            return 1;
        }
    }
    return 0;
}

int find_process_in_queue(processQueueADT queue, uint64_t pid){

    size_t size = getQueueSize(queue);
    int found = 0;
    processCB process;

    for(size_t i=0 ; i<size ; i++){
        process = dequeueProcess(queue);
        if(process.pid == pid && process.state != TERMINATED){
            found = 1;
        }
        addProcessToQueue(queue, process);
        if(found){
            break;
        }
    }

    return found;

}

int is_pid_valid(uint64_t pid){

    if(currentProcess.pid == pid && currentProcess.state != TERMINATED){
        return 1; 
    }

    if(find_process_in_priority_queue(pid)){
        return 1;
    }

    if(find_process_in_queue(allBlockedQueue, pid)){
        return 1;
    }

    return 0;

}

processCB get_process_by_pid(uint64_t pid){

    processQueueADT queues[] = {priority0, priority1, priority2, priority3, allBlockedQueue, NULL};
    processCB process;

    for(int i=0 ; queues[i] != NULL ; i++){
        process = findPidDequeue(queues[i], pid);
        if(process.pid > 0){
            addProcessToQueue(queues[i], process);
            return process;
        }
    }

    return returnNullProcess();
    
}

void waitPid(uint64_t pid){
    if(!is_pid_valid(pid) || !find_process_in_queue(currentProcess.childrenList, pid)){
        return;
    }
    block_process();
}



processCB getCurrentProcess(){
    return currentProcess;
}

void userspaceSetFD(int *fd_ids, int fd_count){
    if(fd_count < 2){
        userspaceProcessCreationFDIds[0] = 0;
        userspaceProcessCreationFDIds[1] = 1;
        userspaceProcessCreationFDCount = 0;
        return;
    }

    int fd_idx;
    for(fd_idx = 0; fd_idx < fd_count; fd_idx++){
        userspaceProcessCreationFDIds[fd_idx] = fd_ids[fd_idx];
    }
    for(; fd_idx < MAX_FD; fd_idx++){
        userspaceProcessCreationFDIds[fd_idx] = 2;
    }
    userspaceProcessCreationFDCount = fd_count;
}

void applyAging(){
    processQueueADT queues[] = {priority0, priority1, priority2, priority3};
    processCB process;
    for(int i = 1 ; i <= HIGHEST_QUEUE ; i++){
        while(hasNextProcess(queues[i])){
            process = dequeueProcess(queues[i]);
            process.priority = i - 1;
            add_priority_queue(process);
        }
    }
}
