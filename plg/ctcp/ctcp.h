#ifndef CTCP_H
#define CTCP_H

#include <stdbool.h>

typedef void (*ctcp_callback)(char *usr, char *src, char *msg,
	char **caps, int ncap);

void ctcp_register(ctcp_callback call, char *pattern);

#endif
