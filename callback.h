#ifndef CALLBACK_H
#define CALLBACK_H

#define MAX_NUMERIC 550 /* looks about right... change this if needed */

typedef void (*callback_numeric)(char *prefix, int ncmd, char *params);
typedef void (*callback_str)(char *prefix, char *cmd, char *params);

void callback_emit_numeric(char *prefix, int ncmd, char *params);
void callback_emit_str(char *prefix, char *cmd, char *params);

void callback_register_numeric(callback_numeric call, int ncmd);
void callback_register_str(callback_str call, char *cmd);

/*
 * callback_init - initialize the callback subsystem
 */
void callback_init();

#endif

