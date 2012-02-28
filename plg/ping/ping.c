#include "callback.h"
#include "ircproto.h"

static void handle_ping(char *prefix, char *cmd, char *params)
{
	ircproto_pong(params);
}

static struct event_str_t events[] = {
	{ handle_ping, "PING" }
};

int init()
{
	callback_register_str(events);
	return 0;
}

void finish()
{
}

