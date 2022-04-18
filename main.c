#include "stdio.h"
#include "assert.h"

#define MAX_INPUT 2048
static char input[MAX_INPUT];

// ----------- parser

#include "ext\mpc.h"
#include "stdlib.h"
#include "errno.h"

enum { LVAL_NUM, LVAL_ERR };
enum { LERR_DIV_ZERO, LERR_BAD_OP, LERR_BAD_NUM };

typedef struct {
	int type;
	long num;
	int err;
} lval;

mpc_parser_t* num;
mpc_parser_t* op;
mpc_parser_t* expr;
mpc_parser_t* cispy;

lval lval_num(long x) {
	lval v = { .type = LVAL_NUM, .num = x };
	return v;
}

lval lval_err(long x) {
	lval v = { .type = LVAL_ERR, .err = x };
	return v;
}

void lval_print(lval v) {
	switch (v.type) {
	case LVAL_NUM:
		printf("%li", v.num);
		break;
	case LVAL_ERR:
		switch (v.err) {
		case LERR_BAD_NUM:
			puts("ERROR: Invalid number");
			break;
		case LERR_BAD_OP:
			puts("ERROR: Invalid operator");
			break;
		case LERR_DIV_ZERO:
			puts("ERROR: Division by zero");
			break;
		
		default:
			assert("Unreachable lval_print 2");
		}
	
		break;
	default:
		assert("Unreachable lval_print");
	}

	putchar('\n');
}

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
lval eval_op(lval l, char* op, lval r) {
	if (l.type == LVAL_ERR) return l;
	if (r.type == LVAL_ERR) return r;

	if (strcmp(op, "+") == 0) return lval_num(l.num+r.num);
	if (strcmp(op, "-") == 0) return  lval_num(l.num-r.num);
	if (strcmp(op, "*") == 0) return  lval_num(l.num*r.num);
	if (strcmp(op, "%") == 0) return  lval_num(l.num%r.num);
	if (strcmp(op, "^") == 0) return  lval_num(pow(l.num, r.num));
	if (strcmp(op, "/") == 0) { 
		if (r.num == 0) {
			return lval_err(LERR_DIV_ZERO);
		}

		return lval_num(l.num/r.num);
	}
	return lval_err(LERR_BAD_OP);
}

/* TODO (ch8)
› How do you give an enum a name?
› Can you use a union in the definition of lval?
› Extend parsing and evaluation to support decimal types using a double field.
*/
lval eval(mpc_ast_t* t) {
	if (strstr(t->tag, "number")) {
		errno = 0;
		long x = strtol(t->contents, NULL, 10);
		if (errno != ERANGE) {
			return lval_num(x);
		}
		return lval_num(LERR_BAD_NUM);
	}

	char* op = t->children[1]->contents;
	lval x = eval(t->children[2]);
	int i = 3;

	while (strstr(t->children[i]->tag, "expr")) {
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
		lval rs = eval(r.output);
		lval_print(rs);
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