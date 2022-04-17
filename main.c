#include "stdio.h"
#define MAX_INPUT 2048
static char input[MAX_INPUT];

// TODO: make repl crossplataform later as in chapter 4

int cmd() { 
	puts("Cispy version 0.1. Press ctr+c to exit.\n");

	while(1) {
		fputs("cispy> ", stdout);
		fgets(input, MAX_INPUT, stdin);

		printf(" %s", input);
	}
}

int main(int argc, char** args) { 
	cmd();
	return 0; 
}