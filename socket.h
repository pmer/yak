#ifndef SOCKET_H
#define SOCKET_H

/* Single, global socket handler. */

#include <stdarg.h>

void establish_connection(char *host, int port);
void close_connection();

void sock_sendline(char *format, ...);
char *sock_readline();

#endif

