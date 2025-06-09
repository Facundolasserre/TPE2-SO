#ifndef _PHILO_H_
#define _PHILO_H_

#include <stdint.h>

#define MAX_PHILOS 10
#define MIN_PHILOS 3
#define INITIAL_THINKERS 5

typedef struct{
    uint64_t pid;
    char *semName; //nombre del semaforo
    int state; // 0 = thinking, 1 = eating
    int hunger;
} Philosopher;


#define MUTEX_PHILO "philoMutex"
#define MUTEX_COUNT_PHILO "countPhiloMutex"
#define MAX_PHILOS 10
#define MIN_PHILOS 3
#define INITIAL_PHILOS 5


void philoSimulation();
void printPhiloState();
uint64_t initPhilosophers(uint64_t argc, char *argv[]);
void eat(int philo);
void think(int philo);
void addPhilosopher(int philo);
void philosopher(uint64_t argc, char *argv[]);

#endif