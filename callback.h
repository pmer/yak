#ifndef CALLBACK_H
#define CALLBACK_H

#define MAX_NUMERIC 550 /* looks about right... change this if needed */

typedef void (*callback_numeric)(char *prefix, int ncmd, char *params);
typedef void (*callback_str)(char *prefix, char *cmd, char *params);

struct event_numeric_t {
	callback_numeric call;
	int ncmd;
};

struct event_str_t {
	callback_str call;
	char *cmd; /* must be statically allocated */
};

void callback_emit_numeric(char *prefix, int ncmd, char *params);
void callback_emit_str(char *prefix, char *cmd, char *params);

#define callback_register_numeric(events) \
	_callback_register_numeric(events, \
		sizeof(events)/sizeof(struct event_numeric_t))
#define callback_register_str(events) \
	_callback_register_str(events, \
		sizeof(events)/sizeof(struct event_str_t))

void _callback_register_numeric(struct event_numeric_t *events, int size);
void _callback_register_str(struct event_str_t *events, int size);

/*
 * callback_init - initialize the callback subsystem
 */
void callback_init();

#endif

