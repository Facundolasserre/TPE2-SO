#ifndef OPEN_FILE_H
#define OPEN_FILE_H

#include <stdint.h>

//struct para representar el descriptor de un archivo abierto
//para gestionar recursos abiertos en el sistema operativo
typedef struct {
    uint64_t id; //id del recurso
    void * resource; //puntero al recurso abiertp
    char (*read)(void *resource); // puntero a la función de lectura específica del recurso
    int (*write)(void *resource, char data);  // puntero a la función de escritura específica del recurso
    int (*close)(); // puntero a la función de cierre específico del recurso
    uint32_t ref_count;     // conteo de referencias para manejo de recursos compartidos
}openFile_t;

#endif 