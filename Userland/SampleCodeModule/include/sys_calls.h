#ifndef _SYSCALLS_H_
#define _SYSCALLS_H_

#include <stdint.h>
#include <colors.h>

/*
 * Pasaje de parametros en C:
   %rdi %rsi %rdx %rcx %r8 %r9
 */

typedef uint64_t (*program_t)(uint64_t argc, char *argv[]);

uint64_t sys_scrWidth();

uint64_t sys_drawRectangle(int x, int y, int x2, int y2, Color color);

uint64_t sys_wait(uint64_t ms);

uint64_t sys_registerInfo(uint64_t reg[17]);

uint64_t sys_printmem(uint64_t mem);

uint64_t sys_read(uint64_t fd);

uint64_t sys_write(uint64_t fd, const char buf);

uint64_t sys_writeColor(uint64_t fd, char buffer, Color color);

uint64_t sys_clear();

uint64_t sys_getHours();

uint64_t sys_pixelPlus();

uint64_t sys_pixelMinus();

uint64_t sys_playSpeaker(uint32_t frequence, uint64_t duration);

uint64_t sys_stopSpeaker();

uint64_t sys_getMinutes();

uint64_t sys_getSeconds();

uint64_t sys_scrHeight();

uint64_t sys_drawCursor();

//memorymanager
void * sys_mem_init(int size);

void * sys_mem_alloc(uint64_t size);

void sys_mem_free(void *ptr);

uint64_t sys_create_process(int priority, program_t program, uint64_t argc, char *argv[]);

uint64_t sys_kill(uint64_t pid);

uint64_t sys_getPID();

uint64_t sys_list_processes(char *buffer);

uint64_t sys_block(uint64_t pid);

uint64_t sys_unblock(uint64_t pid);

uint64_t sys_yield();


//semaphores
uint64_t sys_sem_open(char* sem_name, int init_value);
uint64_t sys_sem_close(void *sem);

uint64_t sys_sem_wait(void *sem);

uint64_t sys_sem_post(void *sem);

//file descriptors
uint64_t sys_read_fd(uint64_t process_fd_index);

uint64_t sys_write_fd(uint64_t process_fd_index, char data);

uint64_t sys_open_fd(uint64_t fd_id);

uint64_t sys_close_fd(uint64_t fd_index);

#endif
