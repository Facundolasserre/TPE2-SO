#ifndef _SHELL_H_
#define _SHELL_H_

#include <stdio.h>
#include "userlib.h"
#define MAX_BUFF 4096
#define MAX_COMMAND 23
#define MAX_ARGS 26
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
void cmd_undefined();
void cmd_help();
void cmd_time();
void cmd_clear();
void cmd_registersinfo();
void cmd_zeroDiv();
void cmd_invOpcode();
void cmd_exit();
void cmd_charsizeplus();
void cmd_charsizeminus();
void cmd_setusername();
void cmd_ascii();
void printPrompt();
void cmd_whoami();
void cmd_eliminator();
void historyCaller(int direction);
void handleSpecialCommands(char c);
void cmd_memoryManagerTest();
void cmd_schetest();
void cmd_priotest();
void cmd_testschedulerprocesses();
void cmd_test_sync();
void cmd_ps();
void cmd_cat();
void cmd_loop();
void cmd_kill();
void cmd_philo();

#endif