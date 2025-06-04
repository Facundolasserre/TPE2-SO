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

uint8_t mutexLock = 1;
int currentSemaphore = 0;



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

void list_processes(char *buffer){
    //8 chars para el state, 3 para el pid, 1 para un espacio y 1 para newline
    buffer = mem_alloc(10 + (get_size(processQueue) + get_size(blockedQueue)) * 13);
    // sprintf(buffer, "PID | STATE\n");
    while(hasNextProcess(processQueue)){
        processCB process = dequeueProcess(processQueue);
        // sprintf(buffer, "%s %.8s", "TODO", process.pid, process.state);
    }
    while(hasNextProcess(blockedQueue)){
        processCB process = dequeueProcess(processQueue);
        // sprintf(buffer, "%s %.8s", "TODO", process.pid, process.state);
    }
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

uint64_t block_process_to_queue(uint64_t pid, processQueueADT blockedQueue){
    processCB process;
   if(currentProcess.pid == pid){
        currentProcess.state = BLOCKED;
   }else if( (process = find_pid_dequeue(processQueue, pid)).pid > 0){
        process.state = BLOCKED;
        add(blockedQueue, process);
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

void initSchedule(){
    currentSemaphore = 0;


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

    sti_asm();
    return currentProcess.rsp; // Retornar el rsp del nuevo proceso
    
}


// Desbloquea el primer proceso esperando en la cola recibida por parametroAdd commentMore actions
uint64_t unblock_process_from_queue(processQueueADT blockedQueue){
    processCB process = dequeueProcess(blockedQueue);
    process.state = READY;
    add(processQueue, process);
}



//SEMAPHORES

typedef struct semaphoreList{
    semaphore_t semaphore;
    struct semaphoreList *next;
} semaphoreList_t;

semaphoreList_t * semList = NULL;


semaphoreList_t * add_semaphore(semaphoreList_t **head, char * name, int initialValue) {
    semaphoreList_t * newSemaphore = (semaphoreList_t*)mem_alloc(sizeof(semaphoreList_t));

    strcpy(newSemaphore->semaphore.name, name, strlen(name));

    newSemaphore->semaphore.value = initialValue;
    newSemaphore->semaphore.lock = 0;
    newSemaphore->semaphore.blockedQueue = new_q();

    if (*head == NULL) {
        *head = newSemaphore;
    } else {
        semaphoreList_t *aux = *head;
        while (aux->next != NULL) {
            aux = aux->next;
        }
        aux->next = newSemaphore;
    }

    return newSemaphore;
}


void remove_sem(semaphoreList_t **head, char * name){
    semaphoreList_t *temp = *head;
    semaphoreList_t *prev = NULL;

    if (temp != NULL && strcmp(temp->semaphore.name, name) == 0) {
        *head = temp->next;
        free_q(temp->semaphore.blockedQueue);
        mem_free(temp);
        return;
    }

    while (temp != NULL && strcmp(temp->semaphore.name, name) != 0) {
        prev = temp;
        temp = temp->next;
    }

    if (temp == NULL) {
        return;
    }

    prev->next = temp->next;

    while(has_next(temp->semaphore.blockedQueue)){
        unblock_process_from_queue(temp->semaphore.blockedQueue);   
    }
    free_q(temp->semaphore.blockedQueue);
    mem_free(temp);
}


semaphoreList_t* find_sem(char * sem_name){
    semaphoreList_t * aux = semList;
    while(aux != NULL){
        if(strcmp(aux->semaphore.name, sem_name) == 0)
            return aux;
        aux = aux->next;
    }
    return NULL;
}










semaphore_t * sem_open(char *sem_name, uint64_t init_value){
    semaphoreList_t * aux = find_sem(sem_name);
    if(aux == NULL)
        aux = add_sem(&semList, sem_name, init_value);
    acquire(aux->semaphore.lock);
    aux->semaphore.value++;
    release(aux->semaphore.lock);
    return 0;
}

void sem_close(char * sem_name){
    semaphoreList_t * aux = find_sem(sem_name);
    if(aux != NULL) {
        acquire(aux->semaphore.lock);
        if(aux->semaphore.value > 0)
            aux->semaphore.value--;
        else
            remove_sem(&semList, sem_name);
        release(aux->semaphore.lock);
    }
    return 0;
}

void sem_wait(char *sem_name){
    semaphoreList_t * sem_node = find_sem(sem_name);
    if(sem_node == NULL) return;
    acquire(sem_node->semaphore.lock);
    if(sem_node->semaphore.value > 0){
        (sem_node->semaphore.value)--;
    } else {
        block_process_to_queue(currentProcess.pid, sem_node->semaphore.blockedQueue);
    }
    release(sem_node->semaphore.lock);
}

void sem_post(char *sem_name){
    semaphoreList_t * sem_node;
    if((sem_node = find_sem(sem_name)) == NULL)
        return 1;

    acquire(sem_node->semaphore.lock);
    if (sem_node->semaphore.value == 0){
        unblock_process_from_queue(sem_node->semaphore.blockedQueue);
    }
    (sem_node->semaphore.value)++;
    release(sem_node->semaphore.lock);
    return 0;
}