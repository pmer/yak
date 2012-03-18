#include "acl.h"
#include "ircproto.h"
#include "plg.h"
#include "privmsg.h"

static void load_lib(char *usr, char *src, char *msg,
		char **caps, int ncaps)
{
	char *lib;

	if (!acl_isowner(usr)) {
		ircproto_privmsg(src, "err: not owner");
		return;
	}
	if (ncaps != 1) {
		ircproto_privmsg(src, "usage: load_lib <lib>");
		return;
	}

	lib = caps[0];
	ircproto_privmsg(src, "loading: %s", lib);
	plg_load(lib);
}

int init()
{
	callback_register_privmsg_re(load_lib,
		"^load_lib ?(\\w+)?(.+)?$");
	return 0;
}

void finish()
{
}

