#include "ircproto.h"

int init()
{
	ircproto_privmsg("#bots", "Hello, world!");
	return 0;
}

void finish()
{
	ircproto_privmsg("#bots", "Farewell.");
}
