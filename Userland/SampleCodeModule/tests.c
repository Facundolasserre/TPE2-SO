#include <userlib.h>
#include <test_util.h>
#include <sys_calls.h>
#include <tests.h>

#define MAX_BLOCKS 128

enum State{
    RUNNING,
    BLOCKED,
    KILLED };

typedef struct Process_rq {
  int32_t pid;
  enum State state;
} process_rq;

typedef struct MemoryManager_rq{
    void *address;
    uint32_t size;
} MemoryManager_rq;

uint64_t mm_test(uint64_t argc, char *argv[]){
    MemoryManager_rq mm_rqs[MAX_BLOCKS];
    uint8_t rq;
    uint32_t total;
    uint64_t max_memory;

    if(argc != 1) return -1;

    if(max_memory = satoi(argv[0]) <= 0) return -1;

    while (1){
        rq = 0;
        total = 0;

        while(rq < MAX_BLOCKS && total < max_memory){
            mm_rqs[rq].size = GetUniform(max_memory - total - 1) + 1;
            mm_rqs[rq].address = sys_mem_alloc(mm_rqs[rq].size);

            if(mm_rqs[rq].address){
                total += mm_rqs[rq].size;
                rq++;
            }
        }

        //Para el Set
        uint32_t i;
        for(i = 0; i < rq; i++){
            if(mm_rqs[i].address) sys_mem_init(mm_rqs[i].size);
        }


        //Para el check
        for(i = 0; i < rq; i++){
            if(mm_rqs[i].address){
                if(!memcheck(mm_rqs[i].address, i, mm_rqs[i].size)){
                    return -1;
                }
            }
        }

        //Para el free
        for(i = 0; i < rq; i++){
            if(mm_rqs[i].address){
                sys_mem_free(mm_rqs[i].address);
            }
        }
    }
}

int64_t test_processes(uint64_t argc, char *argv[]){
    uint8_t rq;
    uint8_t alive = 0;
    uint8_t action;
    uint64_t max_processes;
    char *auxArgv[] = {0};

    if(argc != 1) return -1;

    if((max_processes = satoi(argv[0])) <= 0) return -1;

    process_rq processes_rqs[max_processes];

    while(1){


        prints("======================= Starting test=======================\n", MAX_BUFF);

        for(rq = 0; rq < max_processes; rq++){
            //processes_rqs[rq].pid = sys_create_process("endless_loop", 0, 0, auxArgv);

            processes_rqs[rq].pid = sys_create_process(5, &endless_loop, 0, auxArgv);

            if(processes_rqs[rq].pid == -1){
                prints("test_processes: ERROR creating process\n", MAX_BUFF);
                return -1;
            } else {
                prints("[test_processes] Process created with pid\n", MAX_BUFF);
                processes_rqs[rq].state = RUNNING;
                alive++;
            }
        }

        char buffer[4096];
        sys_list_processes(buffer);
        prints(buffer, 4096);

        while(alive > 0){

            for(rq = 0; rq < max_processes; rq++){
                action = GetUniform(100) % 2;

                switch(action){
                    case 0:
                        if(processes_rqs[rq].state == RUNNING || processes_rqs[rq].state == BLOCKED){
                            if(sys_kill(processes_rqs[rq].pid) == -1){
                                prints("test_processes: ERROR killing process\n", MAX_BUFF);
                                return -1;
                            }
                            prints("[test_processes] Process killed\n", MAX_BUFF);
                            processes_rqs[rq].state = KILLED;
                            alive--;
                        }
                        break;

                    case 1:
                        if (processes_rqs[rq].state == RUNNING) {
                            if (sys_block(processes_rqs[rq].pid) == -1) {
                                prints("test_processes: ERROR blocking process\n", MAX_BUFF);
                                return -1;
                            }
                            processes_rqs[rq].state = BLOCKED;
                            prints("[test_processes] Process blocked\n", MAX_BUFF);

                        }
                        break;
                }
            }

            for(rq = 0; rq < max_processes; rq++){
                if(processes_rqs[rq].state == BLOCKED && GetUniform(100) % 2){
                    if(sys_unblock(processes_rqs[rq].pid) == -1){
                        prints("test_processes: ERROR unblocking process\n", MAX_BUFF);
                        return -1;
                    }
                    prints("[test_processes] Process unblocked\n", MAX_BUFF);
                    processes_rqs[rq].state = RUNNING;
                }
            }
        }
    }
}