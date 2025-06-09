#include <stdint.h>
#include <sys_calls.h>
#include <stdlib.h>
#include <userlib.h>
#include <philo.h>

static Philosopher philos[MAX_PHILOS];
int philoCount;

void philoSimulation(){
    char buffer[MAX_BUFF]={0};
    philoCount = INITIAL_PHILOS;

    write_string("PHILOSOPHER SIMULATION STARTED\nPress 'a' to add a philosopher, 'r' to remove one or 'x' to exit\n", MAX_BUFF);
    wait(99); //esperar 1 segundo para que el usuario lea el mensaje

    if(sys_sem_open(MUTEX_PHILO, 1) != 0 || sys_sem_open(MUTEX_COUNT_PHILO, 1) != 0){
        return;
    }
    for(int i = 0; i < MAX_PHILOS; i++){
        addPhilosopher(i);
    }
    char c = getChar();
    while(c != 'x'){
        if(c == 'a'){
            sys_sem_wait(MUTEX_COUNT_PHILO);

            if(philoCount < MAX_PHILOS){
                philoCount++;
                write_string("Adding philosopher ", MAX_BUFF);
            }
            sys_sem_post(MUTEX_COUNT_PHILO);
        } else if(c == 'r'){
            sys_sem_wait(MUTEX_COUNT_PHILO);

            if(philoCount > MIN_PHILOS){
                philoCount--;
                write_string("Removing philosopher ", MAX_BUFF);
            }
            sys_sem_post(MUTEX_COUNT_PHILO);
        }
        c= getChar();
        
    }

    for(int i=0; i< MAX_PHILOS; i++){
        if(philos[i].pid == 0){
            continue; //si el filosofo no ha sido creado, saltar
        }
        sys_kill(philos[i].pid); //matar el proceso del filosofo
        sys_mem_free(philos[i].semName); //liberar el semaforo del filosofo
        sys_sem_close(philos[i].semName); //cerrar el semaforo del filosofo
        philos[i].semName = NULL;
        philos[i].pid = 0; //resetear el pid del filosofo
    }

    for(int i = 0; i< philoCount; i++){
        write_string("Philosopher ", MAX_BUFF);
        intToStr(i, buffer);
        write_string(buffer, MAX_BUFF);
        write_string(" has eaten ", MAX_BUFF);
        intToStr(philos[i].hunger, buffer);
        write_string(buffer, MAX_BUFF);
        write_string(" times\n", MAX_BUFF);
    }
    
    sys_sem_close(MUTEX_PHILO);
    sys_sem_close(MUTEX_COUNT_PHILO);

    write_string("Philosopher simulation ended\n", MAX_BUFF);


}



void philosopher(uint64_t argc, char *argv[]){
    int id = satoi(argv[0]);
    int left = id;
    int right = (id + 1) % MAX_PHILOS;
    
    while(philoCount != 0){
        if(id % 2 == 0){
            sys_sem_wait(philos[right].semName); //esperar al filosofo de la derecha
            sys_sem_wait(philos[left].semName); //esperar al filosofo de la izquierda
        } else{
            sys_sem_wait(philos[left].semName); //esperar al filosofo de la izquierda
            sys_sem_wait(philos[right].semName); //esperar al filosofo de la derecha
        }

        eat(id); //comer
        sys_sem_post(philos[left].semName); //liberar el filosofo de la izquierda
        sys_sem_post(philos[right].semName); //liberar el filosofo de la derecha

        think(id); //pensar
    }

    sys_kill(sys_getPID()); //matar el proceso del filosofo    
}

void think(int philo){
    sys_sem_wait(MUTEX_PHILO);
    philos[philo].state =  0;
    printPhiloState();
    sys_sem_post(MUTEX_PHILO);
    // sleep(GetUniform(philo));
    sys_wait(GetUniform(philo)); //simular el tiempo de pensamiento
}

void eat(int philo){
    sys_sem_wait(MUTEX_PHILO);
    philos[philo].state = 1;

    if(philo < philoCount){
        philos[philo].hunger++; //incrementar el hambre del filosofo
    }

    printPhiloState();

    sys_sem_post(MUTEX_PHILO);
}



void addPhilosopher(int philo){
    char * indexStr = sys_mem_alloc(sizeof(char)*2);
    intToStr(philo, indexStr); //convertir el id del filosofo a string

    sys_sem_open(indexStr, 1);

    philos[philo].semName = indexStr; //asignar el nombre del semaforo al filosofo

    philos[philo].hunger = 0; //inicializar el hambre del filosofo

    char * philoArgv[2] = {indexStr, NULL};
    philos[philo].pid = create_process(0, (program_t)philosopher, 1, memAllocArgs(philoArgv), NULL, 0); //crear el proceso del filosofo
}





void printPhiloState(){
    char * stateBuffer;

    if(philoCount == 0){
        return;
    }
    sys_sem_wait(MUTEX_COUNT_PHILO);


    for(int i = 0; i < philoCount; i++){
        stateBuffer = philos[i].state == 1 ? "E   " : ".   ";
        write_string(stateBuffer, MAX_BUFF);
    }       
    write_string("\n", MAX_BUFF);
    sys_sem_post(MUTEX_COUNT_PHILO);
}
