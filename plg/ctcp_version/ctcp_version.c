#include "ircproto.h"
#include "yak.h"
#include "plg/ctcp/ctcp.h"

static void ping(char *usr, char *src, char *msg,
		char **caps, int ncaps)
{
	ircproto_notice(src, "\001VERSION yak %s\001", YAK_VERSION);
}

int init()
{
	ctcp_register(ping, "^VERSION$");
	
	return 0;
}

void finish()
{

}

