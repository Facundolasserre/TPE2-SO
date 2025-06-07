#ifndef FILE_DESCRIPTOR_H
#define FILE_DESCRIPTOR_H

#include <stdint.h>
#include <list.h>
#include <openFile.h>

#define STDIN 0
#define STDOUT 1
#define STDERR 2
#define MAX_FD 10

//configuración de la lista de descriptores de archivos 
void initFileDescriptors();

//devuelve el fd para la entrada estándar
openFile_t * getSTDIN_FD();

//devuelve el fd para la salida estándar
openFile_t * getSTDOUT_FD();

//abre un fd segun su id, devuelve el indice del fd abierto
//si no se encuentra el fd devuelve -1
uint64_t openFD(uint64_t fd_id);

//cierra un fd segun su id, decrementa el refCount del fd y si llega a 0 lo elimina y libera la memoria
//devuelve 0 si se cierra correctamente, -1 si no se encuentra el fd o si no se puede cerrar
int closeFD(uint64_t fd_id);

//elimina un fd del proceso actual y libera la memoria asociada
int closeFDCurrentProcess(uint64_t index);

openFile_t * openFDTable(uint64_t fdIds[MAX_FD], int fdCount);

//compara dos fd segun sus id, devuelve la diferencia entre los id
int compareFD( void *fdA,  void *fdB);

int compareFDById(openFile_t *fd, uint64_t id);

//crea un nuevo descriptor de archivo con los recursos y funciones especificadas
openFile_t * createFD(void *resource, char (*read)(void *src), int (*write)(void *dest, char data), int (*close)());

//añade un nuevo descriptor de archivo a la lista de descriptores de archivos
uint64_t addFD(void *resource, char (*read)(), int (*write)(char data), int (*close)());


//elimina un descriptor de archivo de la lista de descriptores de archivos
void removeFD(uint64_t id);

//lee datos de un fd del proceso actual 
char readCurrentProcessFD(uint64_t fdIndex);

//escribe un caracter en el archivo asociado al fd del proceso actual
int writeCurrentProcessFD(uint64_t fdIndex, char data);

openFile_t * openFDTable(uint64_t fdIds[MAX_FD], int fdCount);


#endif //FILE_DESCRIPTOR_H
