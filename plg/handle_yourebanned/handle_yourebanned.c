#include "diagnostic.h"
#include "callback.h"
#include "ircproto.h"

static void handle_yourebanned(char *prefix, int ncmd, char *params)
{
	info("banned from the server");
	want_quit = true;
}

static struct event_numeric_t events[] = {
	{ handle_yourebanned, ERR_YOUREBANNED }
};

int init()
{
	callback_register_numeric(events);
	return 0;
}

void finish()
{
}

