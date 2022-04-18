#include "stdio.h"
#include "assert.h"

#define MAX_INPUT 2048
static char input[MAX_INPUT];

// ----------- parser

#include "ext\mpc.h"

mpc_parser_t* num;
mpc_parser_t* op;
mpc_parser_t* expr;
mpc_parser_t* cispy;

void init_parse() {
	num = mpc_new("number");
	op = mpc_new("operator");
	expr = mpc_new("expression");
	cispy = mpc_new("cispy");
	
	mpca_lang(MPCA_LANG_DEFAULT, 
		"number      : /-?[0-9]+/ ('.' /[0-9]+/)?;" \
		"operator    : '+' | '-' | '*'| '/' | '%' | '^' ;" \
		"expression  : <number> | '(' <operator> <expression>+ ')';" \
		"cispy       : /^/ <operator> <expression>+ /$/;"
	, num, op, expr, cispy);
}

void cleanup_parse() {
	mpc_cleanup(4, num, op, expr, cispy);
}

/* TODO: (ch7)
› Add the function min, which returns the smallest number. For example min 1 5 3 is 1.
› Add the function max, which returns the biggest number. For example max 1 5 3 is 5.
› Change the minus operator - so that when it receives one argument it negates it.
*/
long eval_op(long l, char* op, long r) {
	if (strcmp(op, "+") == 0) return l+r;
	if (strcmp(op, "-") == 0) return l-r;
	if (strcmp(op, "*") == 0) return l*r;
	if (strcmp(op, "/") == 0) return l/r;
	if (strcmp(op, "%") == 0) return l%r;
	if (strcmp(op, "^") == 0) return pow(l, r);
	assert("Unreachable");
	return 0;
}
	
long eval(mpc_ast_t* t) {
	if (strstr(t->tag, "number")) {
		return atoi(t->contents);
	}

	char* op = t->children[1]->contents;
	long x = eval(t->children[2]);
	int i = 3;

	while (strstr(t->children[i]->tag, "expr")) {
		puts("oi\n");
		x = eval_op(x, op, eval(t->children[i]));
		i++;
	}
	
	return x;
}

// ----------- prompt

// TODO: make repl crossplataform as in chapter 4

void eval_input() {
	mpc_result_t r;
	if (mpc_parse("<stdin>", input, cispy, &r)) {
		long rs = eval(r.output);
		printf("%li\n", rs);
		mpc_ast_delete(r.output);
	} else {
		mpc_err_print(r.error);
		mpc_err_delete(r.error);
	}
}

void cmd() { 
	puts("Cispy version 0.1. Press ctr+c or type quit to exit.\n");

	while(1) {
		fputs("cispy> ", stdout);
		fgets(input, MAX_INPUT, stdin);
		eval_input();
	}
}

int main(int argc, char** args) { 
	init_parse();
	cmd();
	cleanup_parse();
	return 0; 
}