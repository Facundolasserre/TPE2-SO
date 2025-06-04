
#include <memoryManager.h>

#define MIN_ALLOC 32

#define TOTAL_MEM 1024*1024

typedef enum {
    FREE,
    DIVIDED,
    FULL
} state_t;

typedef struct node_t{
    struct node_t *left;
    struct node_t *right;
    unsigned index;
    void *mem_ptr;
    state_t state;
    unsigned size;
} node_t;

node_t *root;

#define POW_2(x) (!((x) & ((x) - 1)))

unsigned memory_allocated = 0;

void *rec_alloc(node_t * parent, unsigned s);
int rec_free(node_t *node, void *ptr);
void update_state(node_t *node);
void create_children(node_t *parent);
static unsigned next_power_of_2(unsigned size);

static unsigned next_power_of_2(unsigned size) {
    size |= size >> 1;
    size |= size >> 2;
    size |= size >> 4;
    size |= size >> 8;
    size |= size >> 16;
    return size + 1;
}

void create_children(node_t *parent){
    unsigned idx = parent->index * 2 + 1;

    parent->left = parent + idx;
    if ((uint64_t)parent->left >= MEMORY_START) return;
                                                                            
    parent->left->index = idx;
    parent->left->size = parent->size / 2;
    parent->left->mem_ptr = parent->mem_ptr;
    parent->left->state = FREE;

    parent->right = parent + idx + 1;
    if ((uint64_t)parent->right >= MEMORY_START) return;

    parent->right->index = idx + 1;
    parent->right->size = parent->size / 2;
    
    parent->right->mem_ptr = (void *)((uint64_t)(parent->mem_ptr) + (parent->size / 2));
    parent->right->state = FREE;
}

void update_state(node_t *node){
    if (!node->right || !node->left) {
        node->state = FREE;
        return;
    }
    if (node->left->state == FULL && node->right->state == FULL){
        node->state = FULL;
    }else if (node->left->state == DIVIDED || node->right->state == DIVIDED || node->left->state == FULL || node->right->state == FULL){
        node->state = DIVIDED;
    }
    else{
        node->state = FREE;
    }
}

int rec_free(node_t *node, void *ptr){
    if (!node->left && !node->right && node->state == FULL && node->mem_ptr == ptr){
        node->state = FREE;
        memory_allocated -= node->size;
        return 0;
    }

    int to_ret = -1;
    if (node->left && (uint64_t)node->left->mem_ptr <= (uint64_t)ptr){
        to_ret = rec_free(node->left, ptr);
    }else if (node->right){
        to_ret = rec_free(node->right, ptr);
    }

    update_state(node);
    if (node->state == FREE){
        node->left = NULL;
        node->right = NULL;
    }

    return to_ret;
}

void mem_free(void *ptr){
    if(root){
        rec_free(root, ptr);
    }
}

void mem_init(void *ptr, int s){
    root = (node_t *)ptr;
    root->index = 0;
    root->size = s;
    root->state = FREE;
    root->mem_ptr = (void *)MEMORY_START;
}

void * mem_alloc(uint32_t s){
    if(s > root->size){
        return NULL;
    }

    if(s < MIN_ALLOC){ 
        s = MIN_ALLOC;
    }

    if(!POW_2(s)){
        s = next_power_of_2(s);
    }

    return rec_alloc(root, s);
}

void *rec_alloc(node_t *parent, unsigned s){
    if(parent->state == FULL) return NULL;

    if(parent->left || parent->right){
        void * aux = rec_alloc(parent->left, s);
        if(!aux){ 
            aux = rec_alloc(parent->right, s);
        }
        update_state(parent);
        return aux;
    }else{
        if ( s> parent->size){
            return NULL;
        }
    }

    

    if(parent->size / 2 >= s){
        create_children(parent);
        void * aux = rec_alloc(parent->left, s);
        update_state(parent);
        return aux;
    }
    
    parent->state = FULL;
    memory_allocated += s;
    return parent->mem_ptr;
}