#ifndef SOCKET_H
#define SOCKET_H

/* Single, global socket handler. */

#include <stdarg.h>
#include <stdio.h>

extern FILE *socklog;

void establish_connection(char *host, int port);
void close_connection();

void sock_sendline(char *format, ...);
char *sock_readline();

void sock_init();

#endif

