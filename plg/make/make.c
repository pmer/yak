#include <stdlib.h>

#include "auth.h"
#include "callback.h"
#include "diagnostic.h"
#include "ircproto.h"

static void make(char *usr, char *src, char *msg)
{
	if (!auth_isowner(usr)) {
		ircproto_privmsg(src, "err: not owner");
		return;
	}

	close_connection();
	system("make && ./yak");
	want_quit = true;
}

int init()
{
	callback_register_privmsg_str(make, "make");
	return 0;
}

void finish()
{
}
