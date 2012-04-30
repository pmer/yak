#include "ircproto.h"
#include "yak.h"
#include "pref.h"
#include "stddef.h"
#include "plg/ctcp/ctcp.h"

static void version(char *usr, char *src, char *msg,
		char **caps, int ncaps)
{
	char *version;
	
	version = pref_get("version");
	
	if (version == NULL)
		ircproto_notice(src, "\001VERSION yak %s\001", YAK_VERSION);
	else
		ircproto_notice(src, "\001VERSION %s\001", version);
}

int init()
{
	ctcp_register(version, "^VERSION$");
	
	return 0;
}

void finish()
{

}

