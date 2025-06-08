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
uint64_t foreGroundPID = -2; 
uint64_t userspaceProcessCreationFDIds[MAX_FD] = {0, 1};
uint64_t userspaceProcessCreationFDCount = 0; // Cantidad de FDI de los procesos creados por el usuario

uint64_t agingCounter = 0; // Contador para el envejecimiento de procesos

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

    if(base_pointer == NULL){
        return -1;
    }

    void * stackPointer = fill_stack(base_pointer, initProcessWrapper, program, argc, argv);

    processQueueADT waitingQueue = newProcessQueue();

   openFile_t ** fdTable = openFDTable(fdIds, fdCount);

   int assigned_quantum = ASSIGN_QUANTUM(priority);

    processCB newProcess = {
        PID++,
        base_pointer,
        stackPointer,
        priority,
        assigned_quantum,
        0,
        state,
        fdTable,
        waitingQueue
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
        return returnNullProcess();
    }
    return nextProcess;
}

processCB create_halt_process(){
    void *base_pointer = mem_alloc(STACK_SIZE);
    if(base_pointer == NULL){
         return returnNullProcess();
    }
    void * stack_pointer = fill_stack(base_pointer, initProcessWrapper, &halt, 0, 0);
    processCB new_process = {
                        PID++,    //pid
                        base_pointer,        //base_pointer
                        stack_pointer,      //rsp
                        0,                  //priority
                        0,    //assigned_quantum
                        0,                  //used_quantum
                        HALT,             //state
                        NULL, //fdTable
                        NULL //waitingQueue
                        };
    haltProcess = new_process;
    return new_process;
}


char * list_processes(){

    //tamaño aprox de cada linea seria: (pid + priority + state + separadores y \n)

    const int LINE_SIZE = 68; //tamaño de cada linea
    const int HEADER_SIZE = 68; //tamaño aproximado del encabezado

    char * header = "PID   PRIORITY  STATE  BASE POINTER\n";

    //calculo el tamaño necesario para almacenar todos los procesos
    int totalProcesses = 1; //1 para incluir el proceso actual
    processCB process;
    processQueueADT queues[] = {process0, process1, process2, process3, allBlockedQueue};

    for(int i = 0; i < TOTAL_QUEUES; i++){
        totalProcesses += get_size(queues[i]);
    }

    //asigno memoria para el buffer
    char * buffer = mem_alloc(HEADER_SIZE + (totalProcesses * LINE_SIZE));
    int offset = 0;

    //agrego el proceso actual si esta definido
    if(currentProcess.pid != -1){
        char line[LINE_SIZE];
        formatProcessLine(line, &currentProcess);
        strcpy(buffer + offset, line, LINE_SIZE);
        offset += strlen(line);
    }


    //proceso cada cola de procesos
    for(int i=0 ; i<TOTAL_QUEUES ; i++){
        processQueueADT current = queues[i];
        int size = get_size(current);

        //agrego cada proceso a la cola del buffer
        for(int j=0 ; j<size ; j++){
            process = dequeueProcess(current);
            //formateo cada proceso en una linea
            char line[LINE_SIZE];
            formatProcessLine(line, &process);

            //copio la linea al buffer final
            strcpy(buffer + offset, line, LINE_SIZE);
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
    char * state_str;

    //convierto los enteros a cadena
    intToStr(process->pid, pid_str);
    intToStr(process->priority, priority_str);
    intToStr(process->base_pointer, base_pointer_str);

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
    } else if( (process = find_dequeue_priority(pid)).pid > 0 || (process = find_pid_dequeue(blockedQueue, pid)).pid > 0 ){
        mem_free(process.base_pointer);
    } else {
        return -1;
    }
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
    addProcessToQueue(blockedQueue, process);
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
    
   

    currentProcess = returnNullProcess();
    haltProcess = create_halt_process();

}


uint64_t schedule(void* rsp){

    currentProcess.rsp = rsp; // Actualizar el rsp del proceso actual

    if(++agingCounter >= AGING_THRESHOLD){
        agingCounter = 0;
        applyAging(); // Aplicar envejecimiento a los procesos
    }

    switch(currentProcess.state){
        case RUNNING:
            currentProcess.usedQuantum++;

            if(currentProcess.usedQuantum < currentProcess.assignedQuantum){
                return currentProcess.rsp;
            } else {
                currentProcess.state = READY;
                currentProcess.priority = (currentProcess.priority + 1) > HIGHEST_QUEUE ? HIGHEST_QUEUE : currentProcess.priority + 1;
                currentProcess.assignedQuantum = ASSIGN_QUANTUM(currentProcess.priority);
                currentProcess.usedQuantum = 0;
            
                add_priority_queue(currentProcess); // agregar el proceso actual a la cola
            }
            break;


    case BLOCKED:
        if(++currentProcess.usedQuantum < currentProcess.assignedQuantum){
            currentProcess.priority = (currentProcess.priority + 1) < 0 ? 0 : currentProcess.priority - 1;
            currentProcess.assignedQuantum = ASSIGN_QUANTUM(currentProcess.priority);
        }

        currentProcess.usedQuantum = 0;
        add(allBlockedQueue, currentProcess);
        break;
    

    case READY:
        add_priority_queue(currentProcess);
        break;

    case HALT:
     haltProcess.rsp = rsp;
     break;


    case TERMINATED:
        if(currentProcess.pid == foreGroundPID){
            currentProcess.fdTable[1]->write(currentProcess.fdTable[1]->resource, -1);
            foreGroundPID = -1;
        }


        if(currentProcess.fdTable != NULL){
            for(int i = 0; i < MAX_FD; i++){
                if(currentProcess.fdTable[i] != NULL){
                    fd_close(currentProcess.fdTable[i]->id);
                }
            }
        }
        mem_free(currentProcess.base_pointer);

        while(hasNextProcess(currentProcess.waitingQueue)){
            unblock_process_from_queue(currentProcess.waitingQueue);
        }
        break;

    default:
        // No deberia llegar aca
        break;
    }

    processCB nextProcess = getNextProcess();

    if(nextProcess.state == READY){
        
        currentProcess = nextProcess;
        currentProcess.state = RUNNING;
    }else{
        currentProcess = haltProcess; // Si no hay procesos listos, se ejecuta el proceso de halt

    }
    return currentProcess.rsp;

}

uint64_t userspaceCreateProcessForeground(int priority, program_t program, uint64_t argc, char *argv[]) {
    foreGroundPID = create_process_state(priority, program, READY, argc, argv, userspaceProcessCreationFDIds, userspaceProcessCreationFDCount);
    return foreGroundPID;
}

uint64_t userspaceCreateProcess(int priority, program_t program, uint64_t argc, char *argv[]){
    return create_process_state(priority, program, READY, argc, argv, userspaceProcessCreationFDIds, userspaceProcessCreationFDCount);
}

uint64_t killProcessForeground(){
    if(foreGroundPID < 0){
        return -1;
    }
    kill_process(foreGroundPID);
    uint64_t aux = foreGroundPID; // Guardar el pid antes de cambiarlo
    foreGroundPID = -1;
    return aux; // Retornar el pid del proceso que se mató
   
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
    process.assignedQuantum = ASSIGN_QUANTUM(process.priority);
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

uint64_t set_priority(uint64_t pid, uint8_t priority){
    priority = priority % HIGHEST_QUEUE;
    processCB process;
    if(currentProcess.pid == pid){
        currentProcess.priority = priority;
        currentProcess.assignedQuantum = ASSIGN_QUANTUM(priority);
    } else if ((process = find_dequeue_priority(pid)).pid > 0){
        process.priority = priority;
        add_priority_queue(process);
    } else {
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

    return returnNullProcess();
}

int find_process_in_priority_queue(uint64_t pid){
    processQueueADT queues[] = {processQueue, blockedQueue, blockedReadQueue, blockedSemaphoreQueue};
    for(int i=0 ; i<4 ; i++){
        if(find_process_in_queue(queues[i], pid)){
            return 1;
        }
    }
    return 0;
}

int find_process_in_queue(processQueueADT queue, uint64_t pid){

    size_t size = get_size(queue);
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

    processQueueADT queues[] = {processQueue, blockedQueue, blockedReadQueue, blockedSemaphoreQueue, allBlockedQueue, NULL};
    processCB process;

    for(int i=0 ; queues[i] != NULL ; i++){
        process = find_pid_dequeue(queues[i], pid);
        if(process.pid > 0){
            addProcessToQueue(queues[i], process);
            return process;
        }
    }

    return returnNullProcess();
    
}

void wait_pid(uint64_t pid){
    if(!is_pid_valid(pid)){
        return; // No es un pid valido
    }
    processCB process = get_process_by_pid(pid);
    block_current_process_to_queue(process.waitingQueue);
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

void userspaceSetFD(uint64_t *fd_ids, int fd_count){
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
        userspaceProcessCreationFDIds[fd_idx] = -1;
    }
    userspaceProcessCreationFDCount = fd_count;
}

void applyAging(){
    processQueueADT queues[] = {process0, process1, process2, process3};
    processCB process;
    for(int i = 0; i <= HIGHEST_QUEUE; i++){
        while(hasNextProcess(queues[i])){
            process = dequeueProcess(queues[i]);
            process.priority = i - 1;
            add_priority_queue(process);
        }
    }
}
