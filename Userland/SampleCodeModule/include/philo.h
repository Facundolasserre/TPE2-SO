#ifndef _PHILO_H_
#define _PHILO_H_

#include <stdint.h>

#define MAX_PHILOS 16
#define MIN_PHILOS 2

#define LEFT(i) ((i + num_philos - 1) % num_philos)
#define RIGHT(i) ((i + 1) % num_philos)


typedef enum { THINKING, HUNGRY, EATING } PhiloState;

typedef struct{
    int id;
    PhiloState state;
    uint64_t pid; 
} Philosopher;

#define THINKING 0
#define HUNGRY 1
#define EATING 2

uint64_t initPhilosophers(uint64_t argc, char *argv[]);
void philosopherLoop(uint64_t argc, char *argv[]);
void think(int id);
void eat(int id);
void pickUpForks(int id);
void putDownForks(int id);
void test(int id);
int randomTime();
void createPhilosopherProcess(int id);
void printState();
void addPhilosopher();
void removePhilosopher();
char getNonBlockingChar();

#endif