#include <sys_calls.h>
#include <stdint.h>
#include <userlib.h>
#include <shell.h>

int main(uint64_t argc, char *argv[]) {
	welcome();

	shell();

	return 0;
} 