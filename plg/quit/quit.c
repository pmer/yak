#include "acl.h"
#include "callback.h"
#include "diagnostic.h"
#include "ircproto.h"

static void quit(char *usr, char *src, char *msg)
{
	if (!is_owner_nick(usr)) {
		ircproto_privmsg(src, "err: not owner");
		return;
	}

	want_quit = true;
}

int init()
{
	callback_register_privmsg_str(quit, "quit");
	return 0;
}

void finish()
{
}

