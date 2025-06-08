#include <userlib.h>
#include <stdio.h>
#include <time.h>
#include <sys_calls.h>
#include <colors.h>
#include <eliminator.h>
#include <shell.h>
#include <ascii.h>
#include <tests.h>

// initialize all to 0
char line[MAX_BUFF + 1] = {0};
char parameter[MAX_BUFF + 1] = {0};
char afterPipe[MAX_BUFF + 1] = {0};
char command[MAX_BUFF + 1] = {0};
int commandIdx = 0;
int afterPipeIdx = 0;
int terminate = 0;
int linePos = 0;
char lastc;
static char username[USERNAME_SIZE] = "user";
static char commandHistory[MAX_COMMAND][MAX_BUFF] = {0};
static int commandIterator = 0;
static int commandIdxMax = 0;
static int runInBackground = 0;

uint64_t cursorPID;

char usernameLength = 4;

// Forward declarations
void newLineUsername();
int isUpperArrow(char c);
int isDownArrow(char c);

void printHelp()
{
	printsColor("\n\n    >'help' or 'ls'   - displays this shell information", MAX_BUFF, GREEN);
	printsColor("\n    >setusername        - set username", MAX_BUFF, GREEN);
	printsColor("\n    >whoami             - display current username", MAX_BUFF, GREEN);
	printsColor("\n    >time               - display current time", MAX_BUFF, GREEN);
	printsColor("\n    >clear              - clear the display", MAX_BUFF, GREEN);
	printsColor("\n    >(+)                - increase font size (scaled)", MAX_BUFF, GREEN);
	printsColor("\n    >(-)                - decrease font size (scaled)", MAX_BUFF, GREEN);
	printsColor("\n    >registersinfo      - print current register values", MAX_BUFF, GREEN);
	printsColor("\n    >zerodiv            - testeo divide by zero exception", MAX_BUFF, GREEN);
	printsColor("\n    >invopcode          - testeo invalid op code exception", MAX_BUFF, GREEN);
	printsColor("\n    >eliminator         - launch ELIMINATOR videogame", MAX_BUFF, GREEN);
	printsColor("\n    >mem_test           - testeo del memory manager", MAX_BUFF, GREEN);
	printsColor("\n    >schetest           - test scheduler", MAX_BUFF, GREEN);
	printsColor("\n    >priotest           - priority scheduler", MAX_BUFF, GREEN);
	printsColor("\n    >testschedulerprocesses - test scheduler processes", MAX_BUFF, GREEN);
	printsColor("\n    >testsync           - test sync processes", MAX_BUFF, GREEN);
	printsColor("\n    >ps                 - list all processes", MAX_BUFF, GREEN);
	printsColor("\n    >cat                -cat file", MAX_BUFF, GREEN);
	printsColor("\n    >loop               -prints Pid + greeting to the user", MAX_BUFF, GREEN);
	printsColor("\n    >kill               - kill a process by pid", MAX_BUFF, GREEN);
	printsColor("\n    >philo              - launch philosopher process", MAX_BUFF, GREEN);
	printsColor("\n    >wc					- counts the amount of input lines", MAX_BUFF, GREEN);
	printsColor("\n    >filter             - filter all input vocals", MAX_BUFF, GREEN);
	printsColor("\n    >exit               - exit OS\n", MAX_BUFF, GREEN);

	printc('\n');
}

const char *commands[] = {"undefined", "help", "ls", "time", "clear", "registersinfo", "zerodiv", "invopcode", "setusername", "whoami", "exit", "ascii", "eliminator", "memtest", "schetest", "priotest", "testschedulerprocesses", "testsync", "ps", "cat", "loop", "kill", "philo", "wc", "filter"};
static void (*commands_ptr[MAX_ARGS])() = {cmd_undefined, cmd_help, cmd_help, cmd_time, cmd_clear, cmd_registersinfo, cmd_zeroDiv, cmd_invOpcode, cmd_setusername, cmd_whoami, cmd_exit, cmd_ascii, cmd_eliminator, cmd_memoryManagerTest, cmd_schetest, cmd_priotest, cmd_testschedulerprocesses, cmd_test_sync, cmd_ps, cmd_cat, cmd_loop, cmd_kill, cmd_philo, cmd_wc, cmd_filter};

void shell(){
	welcome();

	char c;
	printPrompt();

	while (1 && !terminate){
		
		c = getChar();
		printLine(c, strcmp(username, "user"));

	}
}

void printLine(char c, int username){
	if (linePos >= MAX_BUFF || c == lastc){
		return;
	}
	if (isChar(c) || c == ' ' || isDigit(c)){
		handleSpecialCommands(c);
	}
	else if (c == BACKSPACE && linePos > 0){
		printc(c);
		line[--linePos] = 0;
	}
	else if (c == NEW_LINE && username)
	{
		processLine();
		prints("\n", MAX_BUFF);
		printPrompt();
		
	}
	else if (c == NEW_LINE && !username){
		newLineUsername();
	}
	lastc = c;
}

void processLine()
{
	checkLine(&commandIdx, &afterPipeIdx);

	prints("\n", MAX_BUFF);

	if (commandIdx && afterPipeIdx){
		pipe_command();
	} else if (commandIdx && runInBackground){
		runInBackground = 0;
		uint64_t fd_table[2] = {2, 2};
		create_process(0, commands_ptr[commandIdx], 0, NULL, fd_table, 2);
	} else if (commandIdx){
		uint64_t pid = create_process_foreground(0, commands_ptr[commandIdx], 0, NULL, NULL, 0);
		sys_wait_pid(pid);
	}

	for (int i = 0; line[i] != '\0'; i++){
		line[i] = 0;
		command[i] = 0;
		parameter[i] = 0;
		afterPipe[i] = 0;
		commandIdx = 0;
		afterPipeIdx = 0;
	}
	linePos = 0;
}

void pipeCommand(){
	uint64_t fdId = sys_pipe_create();
	uint64_t firstPipeFds[2] = {0, fdId};
	uint64_t secondPipeFds[2] = {fdId, 1}; // 1 es STDOUT

	uint64_t pid1 = create_process_foreground(0, commands_ptr[commandIdx], 0, NULL, firstPipeFds, 2);
	create_process(0, commands_ptr[afterPipeIdx], 0, NULL, secondPipeFds, 2);

	sys_wait_pid(pid1);
}

void printPrompt()
{
	prints(username, usernameLength);
	prints(" $", MAX_BUFF);
	printcColor('>', PINK);
}

void cmd_loop(){
	int pid = create_process_foreground(0, &loop_test, 0, NULL, NULL, 0);
	sys_wait_pid(pid);
}

// separa comando de parametro
void checkLine(int * commandIdx, int * afterPipeIdx){
	int i = 0;
	int j = 0;
	int m = 0;
	int k = 0;

	for (j = 0; j < linePos && line[j] != ' '; j++){
		command[j] = line[j];
	}

	while (j < linePos){
		j++;
		if (line[j] == '&'){
			runInBackground = 1;
			break;
		} else if (line[j] == '|'){
			while (line[j] == ' ' || line[j] == '|') j++;

			while (j < linePos) {
				afterPipe[m++] = line[j++];
			}
		} else {
			parameter[k++] = line[j++];
		}
	}

	strcpyForParam(commandHistory[commandIdxMax++], command, parameter);
	commandIterator = commandIdxMax;

	for (i = 1; i < MAX_ARGS; i++) {
		if (strcmp(command, commands[i]) == 0) {
			commandIdx = i;
			if (afterPipe[0] == '\0') {
				return;
			}
			for (int j = 1; j < MAX_ARGS; j++) {
				if (strcmp(afterPipe, commands[j]) == 0) {
					afterPipeIdx = j;
					return;
				}
			}
		}
	}
}

void cmd_setusername(){
	int input_length = strlen(parameter);
	if (input_length < 3 || input_length > USERNAME_SIZE){
		prints("\nERROR: Username length must be between 3 and 16 characters long! Username not set.", MAX_BUFF);
		return;
	}
	usernameLength = input_length;
	for (int i = 0; i < input_length; i++){
		username[i] = parameter[i];
	}
	prints("\nUsername set to ", MAX_BUFF);
	prints(username, usernameLength);
}

void cmd_whoami(){
	prints("\n", MAX_BUFF);
	prints(username, usernameLength);
}

void cmd_cat(){
	uint64_t pid = create_process_foreground(0, &cat, 0, NULL, NULL, 0);
	sys_wait_pid(pid);
}

void cmd_schetest(){
    char *argv[] = {"3"};
    create_process(1, &test_processes, 1, argv, 0, 0);
}

void cmd_priotest(){
	test_prio();
}

void cmd_help()
{
	printsColor("\n\n===== Listing a preview of available commands =====\n", MAX_BUFF, GREEN);
	printHelp();
}

void cmd_undefined()
{
	prints("\n\nbash: command not found: \"", MAX_BUFF);
	prints(command, MAX_BUFF);
	prints("\" Use 'help' or 'ls' to display available commands", MAX_BUFF);
}

void cmd_time()
{
	getTime();
}

void cmd_exit()
{
	prints("\n\nExiting OS\n", MAX_BUFF);
	terminate = 1;
}

void cmd_clear()
{
	clear_scr();
}

void cmd_registersinfo()
{
	registerInfo();
}

void cmd_invOpcode()
{
	test_invopcode();
}

void cmd_zeroDiv()
{
	test_zerodiv();
}

void cmd_charsizeplus()
{
	cmd_clear();
	increaseScale();
	printPrompt();
}

void cmd_charsizeminus()
{
	cmd_clear();
	decreaseScale();
	printPrompt();
}

void handleSpecialCommands(char c)
{
	if (c == PLUS)
	{
		cmd_charsizeplus();
	}
	else if (c == MINUS)
	{
		cmd_charsizeminus();
	}
	else if (isUpperArrow(c))
	{
		historyCaller(-1);
	}
	else if (isDownArrow(c))
	{
		historyCaller(1);
	}
	else
	{
		line[linePos++] = c;
		printc(c);
	}
}

void cmd_ps(){
	char * processes = sys_list_processes();
	prints(processes, MAX_BUFF);
	sys_mem_free(processes);
}

void cmd_eliminator()
{
	int numPlayers;
	if (parameter[0] == '\0')
	{
		numPlayers = 1;
	}
	else
	{
		numPlayers = atoi(parameter);
	}

	if (numPlayers == 1 || numPlayers == 2 || parameter[0] == '\0')
	{
		int playAgain = 1;
		while (playAgain)
		{
			// playAgain because we need to know if the game should be restarted
			playAgain = eliminator(numPlayers);
		}
	}
	else
	{
		prints("\nERROR: Invalid number of players. Only 1 or 2 players allowed.", MAX_BUFF);
	}
}

//testeo del memory manager
void cmd_memoryManagerTest(){

	char *argv[] = {"100000000000000"};

	if (test_mm(1, argv) == -1){
		printsColor("test_mm ERROR\n", MAX_BUFF, RED);
	}
}


void historyCaller(int direction){
	
	for(int i=0 ; i<linePos ; i++){
		printc('\b');
	}

	for(int i=0 ; i<linePos ; i++){
		printc(' ');
	}

	for(int i=0 ; i<linePos ; i++){
		printc('\b');
	}

	commandIterator += direction;

	if(commandIterator < 0){
		commandIterator = 0;
	}else if(commandIterator >= commandIdxMax){
		commandIterator = commandIdxMax - 1;
	}

	strcpy(line, commandHistory[commandIterator]);
	linePos = strlen(commandHistory[commandIterator]);

	prints(commandHistory[commandIterator], MAX_BUFF);

}

void cmd_ascii()
{

	int asciiIdx = random();
	size_t splash_length = 0;
	while (ascii[asciiIdx][splash_length] != NULL)
	{
		splash_length++;
	}

	for (int i = 0; i < splash_length; i++)
	{
		writeString(ascii[asciiIdx][i], MAX_BUFF);
		write_char('\n');
	}
}

void cmd_testschedulerprocesses()
{
	if (test_scheduler_processes() == -1)
	{
		printsColor("test_scheduler_processes ERROR\n", MAX_BUFF, RED);
	}
}

void newLineUsername()
{
	strcpy(username, line);
	usernameLength = strlen(username);

	for (int i = 0; line[i] != '\0'; i++)
	{
		line[i] = 0;
		command[i] = 0;
		parameter[i] = 0;
	}
	linePos = 0;

	prints("\n", MAX_BUFF);
	clear_scr();
}

void cmd_test_sync() {
    char *argv[] = {"5", "1", 0};
	uint64_t pid = create_process_foreground(0, &test_sync, 2, argv, 0, 0);
	sys_wait_pid(pid);
	printsColor("CREATED 'test_sync' PROCESS!\n", MAX_BUFF, RED);
}

void cmd_kill(){
	sys_kill(atoi(parameter));
}

void cmd_philo(){
	init_philosophers(0, NULL);
}

void cmd_wc(){
	create_process_foreground(0, &wc, 0, NULL, NULL, 0);
}

void cmd_filter(){
	create_process_foreground(0, &filter, 0, NULL, NULL, 0);
}

void welcome()
{
	char c;
	prints("\nPlease enter your username: ", MAX_BUFF);
	while (!strcmp(username, "user"))
	{
		c = getChar();
		printLine(c, strcmp(username, "user"));
	}

	

	NoteType windowsXPmelody[] = {
		{622, 300}, // D#5
		{0, 25},
		{466, 300}, // A#4
		{0, 50},
		{415, 450}, // G#4
		{0, 25},
		{622, 250}, // D#5
		{466, 900}	// A#4
	};

	//playMelody(windowsXPmelody, (sizeof(windowsXPmelody) / sizeof(NoteType)));

	printsColor("\n    Welcome to this efficient and simple operating system\n", MAX_BUFF, GREEN);
	printsColor("    Here's a list of available commands\n", MAX_BUFF, GREEN);
	printHelp();

	return 0;
}