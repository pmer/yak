#include "callback.h"
#include "ircproto.h"

static void handle_ping(char *prefix, char *cmd, char *params)
{
	ircproto_pong(params);
}

int init()
{
	callback_register_str(handle_ping, "PING");
	return 0;
}

void finish()
{
}
