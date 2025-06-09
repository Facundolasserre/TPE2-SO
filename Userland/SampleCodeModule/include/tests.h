#ifndef TESTS_H
#define TESTS_H

uint64_t test_mm(uint64_t argc, char *argv[]);

int64_t test_scheduler_processes();
int64_t test_processes(uint64_t argc, char *argv[]);
void first_test_process();
void second_test_process();
void run_test_processes();
void test_prio();
uint64_t test_sync(uint64_t argc, char *argv[]);
void loop_test();
void filter();
void wc();

#endif