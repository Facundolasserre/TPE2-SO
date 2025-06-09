#include <stdint.h>
#include <sys_calls.h>
#include <stdlib.h>
#include <userlib.h>
#include <philo.h>

static Philosopher philos[MAX_PHILOS];
static int thinkers;

void sleep(uint64_t ms){
    sys_wait(ms);
}

uint64_t initPhilosophers(uint64_t argc, char *argv[]){
    sys_sem_open(MUTEX_ARRAY, 1); //crear semaforo para seccion critica
    sys_sem_open(MUTEX_THINKERS, 1); //crear semaforo para controlar el numero de pensadores

    sys_create_process_foreground(0, &controllersHandler, 0, NULL);
        
    thinkers = INITIAL_THINKERS; //numero de pensadores inicial

    for(int i = 0; i < MAX_PHILOS; i++){
        addPhilosopher(i);
    }

    for(int i = 0; i <  MAX_PHILOS; i++){
        sys_wait_pid(philos[i].pid); //esperar a que todos los filosofos terminen
    }
    sys_sem_close(MUTEX_ARRAY);
    sys_sem_close(MUTEX_THINKERS); 

    return 0;
    
}

void philosopherProcess(uint64_t argc, char *argv[]){
    int id = atoi(argv[0]);
    int left = id;
    int right = (id + 1) % MAX_PHILOS;
    
    while(thinkers != 0){
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
    sys_sem_wait(MUTEX_ARRAY);
    philos[philo].state =  0;
    reprint();
    sys_sem_post(MUTEX_ARRAY);
    // sleep(GetUniform(philo));
}

void eat(int philo){
    sys_sem_wait(MUTEX_ARRAY);
    philos[philo].state = 1;
    reprint();
    sys_sem_post(MUTEX_ARRAY);
    // sleep(GetUniform(philo));
}


void addPhilosopher(int philo){
    char indexStr[20];
    intToStr(philo, indexStr); //convertir el id del filosofo a string

    sys_sem_open(indexStr, 1);
    strcpy(philos[philo].semName, indexStr); 

    char * philoArgv[] = {indexStr, NULL};
    philos[philo].pid = sys_create_process(0, (program_t)philosopherProcess, 1, philoArgv); //crear el proceso del filosofo
}

uint64_t controllersHandler(uint64_t argc, char *argv[]){
    while(1){
        char aux = sys_read(0); //leer un caracter del teclado

        switch(aux){
            case 'a':
                sys_sem_wait(MUTEX_THINKERS);
                if(thinkers < MAX_PHILOS)
                    thinkers++; //incrementar el numero de pensadores
                sys_sem_post(MUTEX_THINKERS);
                break;
            case 'r':
                sys_sem_wait(MUTEX_THINKERS);
                if(thinkers > MIN_PHILOS)
                    thinkers--; //decrementar el numero de pensadores
                sys_sem_post(MUTEX_THINKERS);
                break;
            case 'x':
                thinkers = 0;
                return 0;
        }
    }
    return 0;
}

void reprint(){
    if(thinkers == 0)
        return;
    sys_sem_wait(MUTEX_THINKERS);

    for(int i = 0; i < thinkers; i++){
        if(philos[i].state == 1){
            write_string("E   ", MAX_BUFF);
        } else{
            write_string(".   ", MAX_BUFF);
        }
    } 
    write_string("\n\n", MAX_BUFF);
    sys_sem_post(MUTEX_THINKERS);
}
