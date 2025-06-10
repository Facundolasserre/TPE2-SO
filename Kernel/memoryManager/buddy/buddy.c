// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
// Este archivo implementa un administrador de memoria utilizando el algoritmo Buddy System.
// Permite la asignación y liberación de bloques de memoria de manera eficiente.

#include <memoryManager.h>
#include <utils.h>
#include <videoDriver.h>

#define MIN_ALLOC 8
#define TOTAL_MEM 1024*1024



enum StateMem{
    FULL,
    EMPTY,
    SPLIT
};

typedef struct node {
    struct node *left;
    struct node *right;
    unsigned index;
    void *mem_ptr;
    unsigned size;
    enum StateMem state;
} node;



#define POW_2(x) (((x) & ((x) - 1)) == 0)

static node *root;
static unsigned memoryAllocated = 0;
static void traverse_and_print(struct node *node, int depth, char *buf, int *offset);

static unsigned fixsize(unsigned size) {
    size |= size >> 1;
    size |= size >> 2;
    size |= size >> 4;
    size |= size >> 8;
    size |= size >> 16;
    return size + 1;
}



void createSons(node *parent){
    unsigned idx = parent->index * 2 + 1;

    parent->left = (node *)((char *)parent + idx * sizeof(node));
    if ((uint64_t)parent->left >= MEMORY_START){
        return;
    }                                                                 
    parent->left->index = idx;
    parent->left->size = parent->size / 2;
    parent->left->mem_ptr = parent->mem_ptr;
    parent->left->state = EMPTY;
    parent->right =(node *)((char *)parent + (idx + 1) * sizeof(node));
    if ((uint64_t)parent->right >= (uint64_t)MEMORY_START){
        return;
    } 
    parent->right->index = idx + 1;
    parent->right->size = parent->size / 2;
    parent->right->mem_ptr = (void *)((uint64_t)(parent->mem_ptr) + (parent->size / 2));
    parent->right->state = EMPTY;
    return;
}


// Actualiza el estado de un nodo basado en el estado de sus hijos.
void stateUpdate(node *node){
    if (node->right==NULL || node->left==NULL) {
        node->state = EMPTY;
        return;
    }
    if (node->left->state == FULL && node->right->state == FULL){
        node->state = FULL;
    }else if (node->left->state == SPLIT || node->right->state == SPLIT || node->left->state == FULL || node->right->state == FULL){
        node->state = SPLIT;
    }
    else{
        node->state = EMPTY;
    }
}


// Función recursiva para liberar memoria a partir de un nodo.
int freeRec(node *node, void *ptr){
    if (node->left!=NULL || node->right!=NULL){
        int ret;
        if (node->right!=0 && (uint64_t) node->right->mem_ptr > (uint64_t) ptr) {
            ret = freeRec(node->left, ptr);
        } else {
            ret = freeRec(node->right, ptr);
        }
        stateUpdate(node);
        if (node->state == EMPTY) {
            node->right = NULL;
            node->left = NULL;
        }
        return ret;
    } 
    if(node->state == FULL && node->mem_ptr == ptr){
        node->state =  EMPTY;
        memoryAllocated -= node->size;
        return 0;
    }
    return -1;
}

        

// Libera un bloque de memoria dado un puntero.
void mem_free(void *ptr){
    freeRec(root, ptr);
     
}


// Inicializa el administrador de memoria con un puntero y tamaño dados.
void mem_init(void *ptr, uint32_t s){
    root = (node *)ptr;
    root->index = 0;
    root->size = s;
    root->state = EMPTY;
    root->mem_ptr = (void *)MEMORY_START;
}


// Función recursiva para asignar memoria a partir de un nodo padre.
void *allocRec(node *parent, uint32_t s){
    if(parent->state == FULL){
        return NULL;
    }
    if(parent->left || parent->right){
        void * aux = allocRec(parent->left, s);
        if(aux == NULL){ 
            aux = allocRec(parent->right, s);
        }
        stateUpdate(parent);
        return aux;
    }
    if ( s> parent->size){
        return NULL;
    }
    if(parent->size / 2 >= s){
        createSons(parent);
        void * aux = allocRec(parent->left, s);
        stateUpdate(parent);
        return aux;
    }
    parent->state = FULL;
    memoryAllocated += s;
    return parent->mem_ptr;
    return NULL;
}

// Asigna un bloque de memoria de un tamaño dado.
void * mem_alloc(uint32_t s){
    if(s > root->size){
        return NULL;
    }
    if(s < MIN_ALLOC){ 
        s = MIN_ALLOC;
    }
    if(!POW_2(s)){
        s = fixsize(s);
    }
    return allocRec(root, s);
}


char *mem_state() {
    char *buf = mem_alloc(1024*2); 
    if (!buf) {
        return NULL; 
    }
    int offset = 0;
    strcpy(buf + offset, "Memory State:\n", strlen("Memory State:\n"));
    offset += strlen("Memory State:\n");
    // Agrega "Total memory: "
    strcpy(buf + offset, "Total memory: ", strlen("Total memory: "));
    offset += strlen("Total memory: ");
    intToStr(root->size, buf + offset); // Convierte y agrega el tamaño total.
    offset += strlen(buf + offset);
    buf[offset++] = '\n';
    // Agrega "Memory allocated: "
    strcpy(buf + offset, "Memory allocated: ", strlen("Memory allocated: "));
    offset += strlen("Memory allocated: ");
    intToStr(memoryAllocated, buf + offset); // Convierte y agrega la memoria asignada.
    offset += strlen(buf + offset);
    buf[offset++] = '\n';
    // Agrega "Memory free: "
    strcpy(buf + offset, "Memory free: ", strlen("Memory free: "));
    offset += strlen("Memory free: ");
    intToStr(root->size - memoryAllocated, buf + offset); // Convierte y agrega la memoria libre.
    offset += strlen(buf + offset);
    buf[offset++] = '\n';
    traverse_and_print(root, 0, buf, &offset);
    return buf;
}


static void traverse_and_print(node *node, int depth, char *buf, int *offset) {
    if (!node) return;
    // Indentación para mostrar la jerarquía
    for (int i = 0; i < depth; i++) {
        buf[*offset] = ' ';
        (*offset)++;
    }
    // Agrega información del nodo
    strcpy(buf + *offset, "Node ", strlen("Node "));
    (*offset) += strlen("Node ");
    intToStr(node->index, buf + *offset);
    (*offset) += strlen(buf + *offset);
    strcpy(buf + *offset, " | Size: ", strlen(" | Size: "));
    (*offset) += strlen(" | Size: ");
    intToStr(node->size, buf + *offset);
    (*offset) += strlen(buf + *offset);
    strcpy(buf + *offset, " | State: ", strlen(" | State: "));
    (*offset) += strlen(" | State: ");
    strcpy(buf + *offset,
            node->state == EMPTY ? "EMPTY" :
            node->state == SPLIT ? "SPLIT" : "FULL", 
            strlen(node->state == EMPTY ? "EMPTY" :
            node->state == SPLIT ? "SPLIT" : "FULL"));
    (*offset) += strlen(buf + *offset);
    buf[*offset] = '\n';
    (*offset)++;
        // Recursión en los hijos
    traverse_and_print(node->left, depth + 1, buf, offset);
    traverse_and_print(node->right, depth + 1, buf, offset);
}