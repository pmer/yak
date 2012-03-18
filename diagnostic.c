#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "bool.h"
#include "diagnostic.h"

bool want_quit = false;

/**
 * attrs - parameters to VT100 escape sequences
 */
enum attrs {
	CLEAR = 0,
	BOLD = 1,
	LOW_INTENSITY = 2,
	UNDERLINE = 4,
	BLINKING = 5,
	REVERSE_VIDEO = 7,
};

/**
 * txtattr - send a VT100 escape sequence
 */
static void txtattr(FILE *f, int attr)
{
	if (isatty(fileno(f)) == 1)
		fprintf(f, "\033[%dm", attr);
}


/**
 * fn_stack - stack of function pointers
 */
struct fn_stack {
	struct fn_stack *next;
	void (*fn)();
};

/**
 * stack_tail, stack_head - functions to call on safe_shutdown()
 */
static struct fn_stack stack_tail, *stack_head = &stack_tail;


void info(char* format, ...)
{
	va_list args;

	txtattr(stdout, BOLD);
	printf("? ");
	va_start(args, format);
	vprintf(format, args);
	va_end(args);
	putchar('\n');
	txtattr(stdout, CLEAR);
}

void err(char* format, ...)
{
	va_list args;

	txtattr(stderr, REVERSE_VIDEO);
	fprintf(stderr, "! ");
	va_start(args, format);
	vfprintf(stderr, format, args);
	va_end(args);
	fputc('\n', stderr);
	txtattr(stderr, CLEAR);
}

void safe_shutdown()
{
	struct fn_stack *prev, *st = stack_head;
	while (st != &stack_tail) {
		st->fn();
		prev = st;
		st = st->next;
		free(prev);
	}
}

void safe_shutdown_and_die(int code)
{
	safe_shutdown();
	exit(code);
}

void add_shutdown_fn(void (*fn)())
{
	struct fn_stack *st = malloc(sizeof(struct fn_stack));
	st->next = stack_head;
	st->fn = fn;
	stack_head = st;
}
