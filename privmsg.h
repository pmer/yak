#ifndef PRIVMSG_H
#define PRIVMSG_H

#include "regex.h"

#define PRIVMSG_CSTR_BUCKETS 100

typedef void (*callback_privmsg_str)(char *usr, char *src, char *msg);
typedef void (*callback_privmsg_re)(char *usr, char *src, char *msg,
	char **caps, int ncap);

void callback_emit_privmsg(char *usr, char *src, char *msg);

void callback_register_privmsg_str(callback_privmsg_str call, char *str);
void callback_register_privmsg_re(callback_privmsg_re call, char *pattern);

void privmsg_init();

#endif

