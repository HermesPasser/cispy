#include "stdio.h"

// ----------- prompt

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
		"operator    : '+' | '-' | '*'| '/' | '%' ;" \
		"expression  : <number> | '(' <operator> <expression>+ ')';" \
		"cispy       : /^/ <operator> <expression>+ /$/;"
	, num, op, expr, cispy);
}

void cleanup_parse() {
	mpc_cleanup(4, num, op, expr, cispy);
}

// TODO: make repl crossplataform as in chapter 4

void eval_input() {
	mpc_result_t r;
	if (mpc_parse("<stdin>", input, cispy, &r)) {
		mpc_ast_print(r.output);
		mpc_ast_delete(r.output);
	} else {
		mpc_err_print(r.error);
		mpc_err_delete(r.error);
	}
}

int cmd() { 
	puts("Cispy version 0.1. Press ctr+c to exit.\n");

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