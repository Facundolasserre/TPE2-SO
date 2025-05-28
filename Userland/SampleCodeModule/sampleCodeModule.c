#include <sys_calls.h>
#include <stdint.h>
#include <userlib.h>
#include <shell.h>

int main() {
	welcome();

	shell();

	return 0;
} 