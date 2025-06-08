#ifndef _PHILO_H_
#define _PHILO_H_

#include <stdint.h>

#define MAX_PHILOS 10
#define MIN_PHILOS 3
#define INITIAL_THINKERS 5

typedef struct{
    uint64_t pid;
    char semName[20]; //nombre del semaforo
    int state; // 0 = thinking, 1 = eating
} Philosopher;

#define MUTEX_ARRAY "accessArray"
static Philosopher philos[MAX_PHILOS];

#define MUTEX_THINKERS "thinkersMutex"
static int thinkers;

uint64_t initPhilosophers(uint64_t argc, char *argv[]);
uint64_t controllersHandler(uint64_t argc, char *argv[]);
void reprint();
void eat(int philo);
void think(int philo);
void addPhilosopher(int philo);
void philosopherProcess(uint64_t argc, char *argv[]);

#endif