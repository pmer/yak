#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "bool.h"
#include "callback.h"
#include "chan.h"
#include "diagnostic.h"
#include "ircproto.h"
#include "list.h"
#include "plg.h"
#include "privmsg.h"
#include "socket.h"

/*
 * Configuration.
 */
/* char *host = "opsimathia.datnode.net"; */
char *host = "eros.n0v4.com"; /* missing MOTD, convenient */
int port = 6667;

char *bot_nick = "Yak";
char *bot_user = "Yakk";
char *bot_real = "Yakkity yak";
char *bot_identify = NULL;
char *bot_oper_name = NULL;
char *bot_oper_pw = NULL;

char *nickservnick = "NickServ";

char *bot_channels[] = {"#bots"};
int bot_channelcount = 1;

char *bot_owners[] = {"nick"};
int bot_ownercount = 1;

int main()
{
	char *prefix, *cmd, *params;
	int ncmd, i;

	callback_init();
	chan_init();
	privmsg_init();

	info("loading plugins from plugins.txt");
	plg_load_plgs_from("plugins.txt");

	if (want_quit)
		return 0;

	establish_connection(host, port);

	ircproto_nick(bot_nick);
	ircproto_user(bot_user, bot_real);
	if (bot_identify)
		ircproto_identify(nickservnick, bot_identify);
	if (bot_oper_name && bot_oper_pw)
		ircproto_oper(bot_oper_name, bot_oper_pw);

	while (!want_quit) {
		ircproto_read_message(&prefix, &cmd, &ncmd, &params);
		if (ncmd)
			callback_emit_numeric(prefix, ncmd, params);
		else
			callback_emit_str(prefix, cmd, params);

		if (ncmd == RPL_WELCOME) {
			/* distributed on successful user registration */
			for (i = 0; i < bot_channelcount; i++)
				ircproto_join(bot_channels[i]);
		}
	}

	safe_shutdown();
	return 0;
}

