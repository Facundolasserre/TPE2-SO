#include <stdint.h>

typedef struct {
    uint64_t id;                    // Identificador único del recurso
    char (*read)();
    char (*write)(char data);
    int (*close)();                 // Puntero a la función de cierre específico del recurso
    uint32_t ref_count;             // Conteo de referencias para manejo de recursos compartidos
} openFile_t;

