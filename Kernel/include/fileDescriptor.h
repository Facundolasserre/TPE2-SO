#ifndef FILE_DESCRIPTOR_H
#define FILE_DESCRIPTOR_H

#include <stdint.h>
#include <list.h>
#include <openFile.h>


#define STDIN 0             //Id de la entrada estándar
#define STDOUT 1            //Id de la salida estándar
#define DEVNULL 2           //Id del dispositivo nulo (null device)
#define MAX_FD 10           //Número máximo de descriptores de archivos que puede manejar el sistema

//Inicializa la lista de descriptores de archivo y configura los descriptores estándard
void initFileDescriptors();

//devuelve el fd para la entrada estándar
openFile_t * getSTDIN_FD();

//devuelve el fd para la salida estándar
openFile_t * getSTDOUT_FD();

//abre un fd segun su id, devuelve el indice en la tabla del proceso actual
//si no se encuentra el fd, o no hay espacio, devuelve -1
uint64_t openFD(uint64_t fd_id);

//cierra un fd segun su id, decrementa el refCount (contador de referencias) del fd y si llega a 0 lo elimina y libera la memoria
//devuelve 0 si se cierra correctamente, -1 si no se encuentra el fd o si no se puede cerrar
int closeFD(uint64_t fd_id);

//elimina un fd del proceso actual y libera la memoria asociada
//devuelve 0 si se cierra correctamente
int closeFDCurrentProcess(uint64_t index);

//crea una tabla de descriptores de archivos basada en los Ids proporcionados
//retorna NULL si hay error
openFile_t ** openFDTable(uint64_t fdIds[MAX_FD], int fdCount);

//compara dos fd segun sus id, devuelve la diferencia entre los id
int compareFD(void *fdA, void *fdB);

//compara un fd con un id especifico, devuelve la diferencia
int compareFDById(openFile_t *fd, uint64_t id);

//crea un nuevo descriptor de archivo con los recursos y funciones especificadas
openFile_t * createFD(void *resource, char (*read)(void *), int (*write)(void *, char), int (*close)());

//añade un nuevo descriptor de archivo a la lista de descriptores de archivos
uint64_t addFD(void *resource, char (*read)(void *), int (*write)(void *, char), int (*close)());

//elimina un descriptor de archivo de la lista de descriptores de archivos
void removeFD(uint64_t id);

//lee un caracter desde el fd especificado en el proceso actual
//devuelve -1 si el fd no existe
char readFD(uint64_t fdIndex);

//escribe un caracter en el fd especificado en el proceso actual
//devuelve -1 si el fd no existe
int writeFD(uint64_t fdIndex, char data);

//funciones dummy para el dispositivo nulo (null device)
int nullClose();
char nullRead(void *src);
int nullWrite(void *src, char data);


#endif //FILE_DESCRIPTOR_H
