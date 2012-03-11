#include "diagnostic.h"
#include "callback.h"
#include "ircproto.h"

static void handle_yourebanned(char *prefix, int ncmd, char *params)
{
	info("banned from the server");
	want_quit = true;
}

int init()
{
	callback_register_numeric(handle_yourebanned, ERR_YOUREBANNED);
	return 0;
}

void finish()
{
}

