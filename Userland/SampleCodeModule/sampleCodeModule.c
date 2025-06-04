#include <sys_calls.h>
#include <stdint.h>
#include <userlib.h>
#include <shell.h>
#include <tests.h>

int main(uint64_t argc, char *argv[]) {
	
	shell();
	
	return 0;
} 
