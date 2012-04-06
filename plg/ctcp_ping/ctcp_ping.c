#include "ircproto.h"
#include "plg/ctcp/ctcp.h"

static void ping(char *usr, char *src, char *msg,
		char **caps, int ncaps)
{
	ircproto_notice(src, "\001PING %s\001", caps[0]);
}

int init()
{
	ctcp_register(ping, "^PING (.*)$");
	
	return 0;
}

void finish()
{

}

