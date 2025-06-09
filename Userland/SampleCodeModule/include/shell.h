#ifndef _SHELL_H_
#define _SHELL_H_

#include <stdio.h>
#include "userlib.h"
#define MAX_COMMAND 256
#define MAX_ARGS 29
#define USERNAME_SIZE 16
#define NEW_LINE '\n'
#define BACKSPACE '\b'
#define PLUS '+'
#define MINUS '-'

void shell();

void printHelp();
void processLine();
void printLine(char c, int username);
void checkLine(int * commandIdx, int * afterPipeIdx);
uint64_t cmd_undefined(uint64_t argc, char * argv[]);
uint64_t cmd_help(uint64_t argc, char * argv[]);
uint64_t cmd_time(uint64_t argc, char * argv[]);
uint64_t cmd_clear(uint64_t argc, char * argv[]);
uint64_t cmd_registersinfo(uint64_t argc, char * argv[]);
uint64_t cmd_zeroDiv(uint64_t argc, char * argv[]);
uint64_t cmd_invOpcode(uint64_t argc, char * argv[]);
uint64_t cmd_exit(uint64_t argc, char * argv[]);
uint64_t cmd_charsizeplus(uint64_t argc, char * argv[]);
uint64_t cmd_charsizeminus(uint64_t argc, char * argv[]);
uint64_t cmd_setusername(uint64_t argc, char * argv[]);
uint64_t cmd_ascii(uint64_t argc, char * argv[]);
void printPrompt();
uint64_t cmd_whoami(uint64_t argc, char * argv[]);
uint64_t cmd_eliminator(uint64_t argc, char * argv[]);
uint64_t cmd_memoryManagerTest(uint64_t argc, char * argv[]);
void historyCaller(int direction);
void handleSpecialCommands(char c);
uint64_t cmd_schetest(uint64_t argc, char * argv[]);
uint64_t cmd_priotest(uint64_t argc, char * argv[]);
uint64_t cmd_run_test_processes(uint64_t argc, char * argv[]);
uint64_t cmd_test_sync(uint64_t argc, char * argv[]);
uint64_t cmd_ps(uint64_t argc, char * argv[]);
uint64_t cmd_cat(uint64_t argc, char * argv[]);
uint64_t cmd_loop(uint64_t argc, char * argv[]);
uint64_t cmd_kill(uint64_t argc, char * argv[]);
uint64_t cmd_philo(uint64_t argc, char * argv[]);
uint64_t cmd_filter(uint64_t argc, char * argv[]);
uint64_t cmd_wc(uint64_t argc, char * argv[]);
uint64_t cmd_block(uint64_t argc, char * argv[]);
uint64_t cmd_unblock(uint64_t argc, char * argv[]);
uint64_t cmd_nice(uint64_t argc, char * argv[]);
uint64_t cmd_mem(uint64_t argc, char * argv[]);
void pipeCommand();

#endif