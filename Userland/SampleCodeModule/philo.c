#include <stdint.h>
#include <sys_calls.h>
#include <stdio.h>
#include <stdlib.h>
#include <userlib.h>
#include <philo.h>

Philosopher philos[MAX_PHILOS];
int num_philos = 5; //numero inicial de filosofos
void * mutex; //semaforo para seccion critica
void * semaphores[MAX_PHILOS]; //semaforos para cada filosofo

uint64_t initPhilosophers(uint64_t argc, char *argv[]){
    mutex = sys_sem_open("mutex", 1);
    for(int i=0 ; i<num_philos ; i++){
        char sem_name[20];
        intToStr(i, sem_name);
        semaphores[i] = sys_sem_open(sem_name, 0);
        
        philos[i].id = i;
        philos[i].state = THINKING;

        createPhilosopherProcess(i);
    }

    write_string("Press 'a' to add a philosopher, 'r' to remove, 'q' to quit.\n", MAX_BUFF);

    int running = 1;
    while(running){
        char input = getNonBlockingChar();
        
        if(input == 'a'){
            addPhilosopher();
        }else if(input == 'r'){
            removePhilosopher();
        }else if(input == 'q'){
            running = 0;
        }

        printState();
        while (sys_getSeconds() % 2 != 0);
    }

    for(int i=0 ; i<num_philos ; i++){
        sys_kill(philos[i].pid);
        char sem_name[20];
        intToStr(i, sem_name);
        sys_sem_close(semaphores[i]);
    }

    sys_sem_close(mutex);
    return 0;
    
}

void philosopherLoop(uint64_t argc, char *argv[]){
    int id = atoi(argv[0]);
    
    while(1){
        think(id);
        pickUpForks(id);
        eat(id);
        putDownForks(id);
    }
}

void think(int id){
    while(sys_getSeconds() % 2 != 0);
}

void eat(int id){
    while(sys_getSeconds() % 2 != 0);
}

void pickUpForks(int id){
    sys_sem_wait(mutex);
    philos[id].state = HUNGRY;
    test(id);
    sys_sem_post(mutex);
    sys_sem_wait(semaphores[id]);
}

void putDownForks(int id){
    sys_sem_wait(mutex);
    philos[id].state = THINKING;
    test(LEFT(id)); //notificar al filosofo de la izquierda
    test(RIGHT(id)); //notificar al filosofo de la derecha
    sys_sem_post(mutex);
}

void test(int id){
    if(philos[id].state == HUNGRY && 
       philos[LEFT(id)].state != EATING &&
       philos[RIGHT(id)].state != EATING){
        philos[id].state = EATING;
        sys_sem_post(semaphores[id]); //notificar que el filosofo puede comer
       }
}

int randomTime(){
    return 500 + (sys_getPID() % 500); // tiempo aleatorio entre 500 y 1000 ms
}

void createPhilospherProcess(int id){
    char buffer[10];
    intToStr(id, buffer);
    char * argv[] = {buffer, NULL};
    uint64_t pid = sys_create_process(1, philosopherLoop, 1, argv);
    philos[id].pid = pid;
}

void printState(){
    sys_sem_wait(mutex); //bloqueo para seccion critica
    for(int i = 0; i < num_philos; i++){
        if(philos[i].state == EATING){
            write_char('E');
        } else {
            write_char('.');
        }
    }
    write_char("\n");
    sys_sem_post(mutex); //desbloqueo
}


void addPhilosopher(){
    if(num_philos >= MAX_PHILOS){
        write_string("Maximum number of philosophers reached.\n", MAX_BUFF);
        return;
    }

    sys_sem_wait(mutex); //bloqueo

    int id = num_philos; //id del nuevo filosofo
    num_philos++;
    
    char sem_name[20];
    intToStr(id, sem_name);
    semaphores[id] = sys_sem_open(sem_name, 0); //crear semaforo para el nuevo filosofo

    philos[id].id = id;
    philos[id].state = THINKING;

    createPhilosopherProcess(id); //crear proceso para el nuevo filosofo

    sys_sem_post(mutex); //desbloqueo
}

void removePhilosopher(){
    if(num_philos <= MIN_PHILOS){
        write_string("Minimum number of philosophers reached.\n", MAX_BUFF);
        return;
    }
    sys_sem_wait(mutex);
    num_philos--;
    int id = num_philos; //id del ultimo filosofo

    sys_kill(philos[id].pid); //matar el proceso del ultimo filosofo
    sys_sem_close(semaphores[id]); //cerrar el semaforo del ultimo filosofo

    sys_sem_post(mutex);
}

char getNonBlockingChar(){
    char c;
    if((c = sys_read(0)) > 0){
        return c;
    } else {
        return 0;
    }
}
