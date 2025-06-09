// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
#include <userlib.h>
#include <test_util.h>
#include <sys_calls.h>
#include <tests.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>

#define MAX_BLOCKS 128
#define MINOR_WAIT 1000000 // TODO: Change this value to prevent a process from flooding the screen
#define WAIT 10000000      // TODO: Change this value to make the wait long enough to see theese processes beeing run at least twice

#define TOTAL_PROCESSES 3
#define LOWEST 0  // TODO: Change as required
#define MEDIUM 1  // TODO: Change as required
#define HIGHEST 3 // TODO: Change as required

#define SEM_ID "sem"
#define TOTAL_PAIR_PROCESSES 2


enum State { RUNNING,
             BLOCKED,
             KILLED };

typedef struct P_rq {
  int32_t pid;
  enum State state;
} p_rq;

typedef struct MM_rq {
  void *address;
  uint32_t size;
} mm_rq;

int64_t prio[TOTAL_PROCESSES] = {LOWEST, MEDIUM, HIGHEST};

static void wait_03(uint64_t pid1, uint64_t pid2);
static void wait_30(uint64_t pid1, uint64_t pid2);


uint64_t test_mm(uint64_t argc, char *argv[]) {

  mm_rq mm_rqs[MAX_BLOCKS];
  uint8_t rq;
  uint32_t total;
  uint64_t max_memory;

  if (argc != 1)
    return -1;

  if ((max_memory = satoi(argv[0])) <= 0)
    return -1;

  while (1) {
    rq = 0;
    total = 0;

    // Request as many blocks as we can
    while (rq < MAX_BLOCKS && total < max_memory) {
      write_string("Requesting memory block...\n", strlen("Requesting memory block...\n"));
      printDec(mm_rqs[rq].size);
      write_string(" bytes\n", strlen(" bytes\n"));
      mm_rqs[rq].size = GetUniform(max_memory - total - 1) + 1;
      mm_rqs[rq].address = sys_mem_alloc(mm_rqs[rq].size);

      if (mm_rqs[rq].address) {
        total += mm_rqs[rq].size;
        rq++;
      }
    }

    // Set
    uint32_t i;
    for (i = 0; i < rq; i++)
      if (mm_rqs[i].address)
        sys_mem_init(mm_rqs[i].size);

    // Check
    for (i = 0; i < rq; i++)
      if (mm_rqs[i].address)
        if (!memcheck(mm_rqs[i].address, i, mm_rqs[i].size)) {
          return -1;
        }

    // Free
    for (i = 0; i < rq; i++)
      if (mm_rqs[i].address)
        sys_mem_free(mm_rqs[i].address);
  }
}

int64_t test_processes(uint64_t argc, char *argv[]) {
  uint8_t rq;
  uint8_t alive = 0;
  uint8_t action;
  uint64_t max_processes;
  char *argvAux[] = {0};

  if (argc != 1)
    return -1;

  if ((max_processes = satoi(argv[0])) <= 0)
    return -1;

  p_rq p_rqs[max_processes];

  while (1) {
    prints("======================= Starting test=======================\n", MAX_BUFF);
    // Create max_processes processes
    for (rq = 0; rq < max_processes; rq++) {
      // p_rqs[rq].pid = sys_create_process("endless_loop", 0, 0, argvAux);
      p_rqs[rq].pid = create_process(0, (program_t)endless_loop, 0, argvAux, 0, 0);

      if (p_rqs[rq].pid == -1) {
        prints("test_processes: ERROR creating process\n", MAX_BUFF);
        return -1;
      } else {
        prints("[test_processes] Process created with pid\n", MAX_BUFF);
        p_rqs[rq].state = RUNNING;
        alive++;
      }
    }

    char buffer[4096];
    sys_list_processes(buffer);
    prints(buffer, 4096);

    // Randomly kills, blocks or unblocks processes until every one has been killed
    while (alive > 0) {

      for (rq = 0; rq < max_processes; rq++) {
        action = GetUniform(100) % 2;

        switch (action) {
          case 0:
            if (p_rqs[rq].state == RUNNING || p_rqs[rq].state == BLOCKED) {
              if (sys_kill(p_rqs[rq].pid) == -1) {
                prints("test_processes: ERROR killing process\n", MAX_BUFF);
                return -1;
              }
              prints("[test_processes] Process killed\n", MAX_BUFF);
              p_rqs[rq].state = KILLED;
              alive--;
            }
            break;

          case 1:
            if (p_rqs[rq].state == RUNNING) {
              if (sys_block(p_rqs[rq].pid) == -1) {
                prints("test_processes: ERROR blocking process\n", MAX_BUFF);
                return -1;
              }
              p_rqs[rq].state = BLOCKED;
              prints("[test_processes] Process blocked\n", MAX_BUFF);
            }
            break;
        }
      }

      // Randomly unblocks processes
      for (rq = 0; rq < max_processes; rq++)
        if (p_rqs[rq].state == BLOCKED && GetUniform(100) % 2) {
          if ((p_rqs[rq].pid) == -1) {
            prints("test_processes: ERROR unblocking process\n", MAX_BUFF);
            return -1;
          }
          prints("[test_processes] Process unblocked\n", MAX_BUFF);
          p_rqs[rq].state = RUNNING;
        }
    }
  }
}

void test_process(uint64_t argc, char *argv[]) {
  if (argc != 1) {
    prints("test_process: ERROR: Invalid number of arguments\n", MAX_BUFF);
    return;
  }
  uint64_t n = satoi(argv[0]);

  while(1){
    
    switch(n){
      case 0:
        prints("++++++++++++++++++++++++++++++\n", MAX_BUFF);
        break;
      case 1:
        prints("------------------------------\n", MAX_BUFF);
        break;
      case 2:
        prints("******************************\n", MAX_BUFF);
        break;
      case 3:
        prints("/ / / / / / / / / / / / / / / / /\n", MAX_BUFF);
        break;
      case 4:
        prints(":::::::::::::::::::::::::::::::\n", MAX_BUFF);
        break;
      case 5:
        prints("0000000000000000000000000000000\n", MAX_BUFF);
        break;
      default:
        prints("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\n", MAX_BUFF);
        break;
    }
  }
}

int64_t test_scheduler_processes() {
  uint8_t rq;
  uint8_t alive = 0;
  uint64_t max_processes = 5;

  p_rq p_rqs[max_processes];

  prints("======================= CREATING TEST PROCESSES!=======================\n", MAX_BUFF);
    // Create max_processes processes
    for (rq = 0; rq < max_processes; rq++) {
      // p_rqs[rq].pid = sys_create_process("endless_loop", 0, 0, argvAux);
      char *argv[] = {(char*)(uint64_t)rq};
      p_rqs[rq].pid = create_process(0, (program_t)test_process, 0, argv, 0, 0);

      if (p_rqs[rq].pid == -1) {
        prints("ERROR creating process\n", MAX_BUFF);
        return -1;
      } else {
        prints("Process created with pid\n", MAX_BUFF);
        p_rqs[rq].state = RUNNING;
        alive++;
      }
    }

    char buffer[4096];
    sys_list_processes(buffer);
    prints(buffer, 4096);

    return 0;
}

void test_prio() {
  int64_t pids[TOTAL_PROCESSES];
  char *argv[] = {0};
  uint64_t i;

  for (i = 0; i < TOTAL_PROCESSES; i++)
    pids[i] = create_process(0, (program_t)&endless_loop, 0, argv, 0, 0);

  busy_wait(WAIT);
  prints("\nCHANGING PRIORITIES...\n", MAX_BUFF);

  for (i = 0; i < TOTAL_PROCESSES; i++)
    sys_nice(pids[i], prio[i]);

  busy_wait(WAIT);
  prints("\nBLOCKING...\n" ,MAX_BUFF);

  for (i = 0; i < TOTAL_PROCESSES; i++)
    sys_block(pids[i]);

  prints("CHANGING PRIORITIES WHILE BLOCKED...\n",MAX_BUFF);

  for (i = 0; i < TOTAL_PROCESSES; i++)
    sys_nice(pids[i], MEDIUM);

  prints("UNBLOCKING...\n",MAX_BUFF);

  for (i = 0; i < TOTAL_PROCESSES; i++)
    sys_unblock(pids[i]);

  busy_wait(WAIT);
  prints("\nKILLING...\n",MAX_BUFF);

  for (i = 0; i < TOTAL_PROCESSES; i++)
    sys_kill(pids[i]);
}


static void wait_03(uint64_t pid1, uint64_t pid2){
    wait(5);
    sys_nice(pid1, 0);
    sys_nice(pid2, 3);
}

static void wait_30(uint64_t pid1, uint64_t pid2){
    wait(5);
    sys_nice(pid1, 3);
    sys_nice(pid2, 0);
}




//SEMAPHORES

int64_t global ; // shared memory

void slowInc(int64_t *p, int64_t inc) {
  int64_t aux = *p;
  sys_yield(); // This makes the race condition highly probable
  aux += inc;
  *p = aux;
}

uint64_t my_process_inc(uint64_t argc, char *argv[]) {
  uint64_t n;
  int8_t inc;
  int8_t use_sem = 1;
  

  if (argc != 3)
    return -1;

  if ((n = satoi(argv[0])) <= 0)
    return -1;
  if ((inc = satoi(argv[1])) == 0)
    return -1;
  if ((use_sem = satoi(argv[2])) < 0)
    return -1;

  if (use_sem)
    if (sys_sem_open(SEM_ID, 1)) {
      prints("test_sync: ERROR opening semaphore\n", MAX_BUFF);
      return -1;
    }

  uint64_t i;
  for (i = 0; i < n; i++) {
    if (use_sem)
      sys_sem_wait(SEM_ID);
    slowInc((int64_t *)&global, inc);
    write_string("Process: ", strlen("Process: "));
    printDec(sys_getPID());
    write_string(" Global: ", strlen(" Global: "));
    
    if(global < 0){
      write_string("-", strlen("-"));
      printDec(-global);
    } else {
      printDec(global);
    }

    write_string("\n", strlen("\n"));
    if (use_sem)
      sys_sem_post(SEM_ID);
  }

  if (use_sem)
    sys_sem_close(SEM_ID);

  return 0;
}





uint64_t test_sync(uint64_t argc, char *argv[]) { //{n, use_sem, 0}
  uint64_t pids[2 * TOTAL_PAIR_PROCESSES];

  if (argc != 2)
    return -1;

  char *argvDec[] = {"5", "-1", "1", NULL};
  char *argvInc[] = {"5", "1", "1", NULL};

  

  uint64_t i;
  for (i = 0; i < TOTAL_PAIR_PROCESSES; i++) {
    pids[i] = create_process(0, &my_process_inc, 3, memAllocArgs(argvDec), NULL, 0);
    pids[i + TOTAL_PAIR_PROCESSES] = create_process(0, &my_process_inc, 3, memAllocArgs(argvInc), NULL, 0);
  }

  for(i=0 ; i<TOTAL_PAIR_PROCESSES ; i++){
    prints("Waiting for process with PID: ", strlen("Waiting for process with PID: "));
    printDec(pids[i]);
    prints("\n", strlen("\n"));
    sys_wait_pid(pids[i]);

    prints("Waiting for process with PID: ", strlen("Waiting for process with PID: "));
    printDec(pids[i + TOTAL_PAIR_PROCESSES]);
    prints("\n", strlen("\n"));
    sys_wait_pid(pids[i + TOTAL_PAIR_PROCESSES]);
  }

  prints("Final value: ", strlen("Final value: "));
  printDec(global);
  prints("\n", strlen("\n"));

  return 0;
}

void first_test_process(){
  while(1){
    write_char('A');
  }
}

void second_test_process(){
  while(1){
    write_char('-');
  }
}

void run_test_processes(){
  write_string("Running test_processes\n Press 1 or 2 to change process priorities or C to stop.", strlen("Running test_processes\n Press 1 or 2 to change process priorities or C to stop."));
  wait(99);
  uint64_t pid1 = create_process(0, (program_t)first_test_process, 0, NULL, NULL, 0);     // PID 1 == 'A'
	uint64_t pid2 = create_process(0, (program_t)second_test_process, 0, NULL, NULL, 0);    // PID 2 == '-'
  char c;
	while(1){
    c = getChar();
    if(c == '1' || c == '2'){
      sys_block(pid1);
      sys_block(pid2);
    }
   if(c == '1'){
      sys_nice(pid1, 0);
      sys_nice(pid2, 3);
      write_string("Process A now has lowest priority and process - has highest priority\n", strlen("Process A now has lowest priority and process + has highest priority\n"));
      char *processes = sys_list_processes();
      write_string(processes, MAX_BUFF);
      sys_mem_free(processes);
      wait(99);
      sys_unblock(pid1);
      sys_unblock(pid2);
      wait_03(pid1, pid2);
      wait_03(pid1, pid2);
    } else if(c == '2'){
      sys_nice(pid1, 3);
      sys_nice(pid2, 0);
      write_string("Process - now has lowest priority and process A has highest priority\n", strlen("Process + now has lowest priority and process A has highest priority\n"));
      char *processes = sys_list_processes();
      write_string(processes, MAX_BUFF);
      sys_mem_free(processes);
      wait(99);
      sys_unblock(pid1);
      sys_unblock(pid2);
      wait_30(pid1, pid2);
      wait_30(pid1, pid2);
    } else if(c == 'C' || c == 'c'|| c == 'q' || c == 'Q'){
      sys_kill(pid1);
      sys_kill(pid2);
      write_string("Test_processes finished\n", strlen("Test_processes finished\n"));
      break;
    }
	}
}
