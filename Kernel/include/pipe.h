#ifndef PIPE_H
#define PIPE_H

#include <stdint.h>
#include <semaphore.h>

#define BUFFER_SIZE 4096        //tamaño del buffer circular del pipe
#define SEMAPHORE_NAME_SIZE 32  //tamaño max del nombre de los semáforos

typedef struct pipe{
    uint16_t id;                    //identificador del pipe
    char * buffer;                  //buffer circular para almacenar los datos
    uint16_t readIndex;             //índice de lectura del buffer
    uint16_t writeIndex;            //índice de escritura del buffer
    char * sem_name_data_available; //nombre del semáforo que indica si hay datos disponibles para leer
    char * sem_name_mutex;          //nombre del semáforo para exclusión mutua
}pipe;

//Inicializa la escritura de pipes en el sistema
int initPipes();

//Crea un nuevo pipe y devuelve su identificador como fd
uint64_t pipeCreate();

//Destruye un pipe dado su identificador
void pipeDestroy(uint16_t pipeId);

//Lee un carácter del pipe dado su identificador
char pipeRead(uint16_t pipeId);

//Escribe un carácter en el pipe dado su identificador
int pipeWrite(uint16_t pipeId, char c);

//Compara dos pipes por su identificador
int comparePipes(void *a, void *b);

//Inicializa un nuevo pipe
pipe * pipeInit();

//Libera la memoria asociada a un pipe
void freePipe(pipe * p);

//Genera un nombre de semáforo basado en un nombre base y un identificador
void getSemaphoreName(const char * baseName, int id, char * dest);

#endif