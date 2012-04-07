#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "auth.h"
#include "bool.h"
#include "callback.h"
#include "chan.h"
#include "diagnostic.h"
#include "ircproto.h"
#include "list.h"
#include "plg.h"
#include "pref.h"
#include "privmsg.h"
#include "socket.h"
#include "thread.h"
#include "usr.h"

char *host;
char *port;

char bot_nick[512];
char *bot_user;
char *bot_real;
char *nickservnick;
char *bot_identify;
char *bot_oper_name;
char *bot_oper_pw;

char **bot_channels;
char **bot_owners;

static void load_whoiam()
{
	char *owners_str, *chan_str, *tok;
	int i;

	host = pref_get("host");
	port = pref_get("port");
	strcpy(bot_nick, pref_get("nick"));
	bot_user = pref_get("user");
	bot_real = pref_get("real");
	nickservnick = pref_get("nickserv-nick");
	bot_identify = pref_get("identify-pw");
	bot_oper_name = pref_get("oper-name");
	bot_oper_pw = pref_get("oper-pw");
	owners_str = pref_get("owners");
	chan_str = pref_get("channels");

	bot_owners = NULL;
	for (i = 0, tok = strtok(owners_str, " "); tok; i++, tok = strtok(NULL, " ")) {
		bot_owners = realloc(bot_owners, sizeof(char*) * (i + 2));
		bot_owners[i] = tok;
	}
	bot_owners[i] = NULL;

	/* move to plugin */
	bot_channels = NULL;
	for (i = 0, tok = strtok(chan_str, " "); tok; i++, tok = strtok(NULL, " ")) {
		bot_channels = realloc(bot_channels, sizeof(char*) * (i + 2));
		bot_channels[i] = tok;
	}
	bot_channels[i] = NULL;
}

int main()
{
	char *prefix, *cmd, *params;
	char **chans;
	int ncmd, i;

	/* lock the global mutex during program startup */
	nthreads++;
	mutex_on();

	pref_init();
	load_whoiam();

	callback_init();
	sock_init();
	usr_init();
	chan_init();
	auth_init();
	privmsg_init();

	info("loading plugins from plugins.txt");
	plg_load_plgs_from("plugins.txt");

	mutex_off();
	nthreads--;

	/* maybe something drastic happened during startup */
	if (want_quit)
		goto shutdown;

	establish_connection(host, port);

	ircproto_nick(bot_nick);
	ircproto_user(bot_user, bot_real);
	if (bot_identify)
		ircproto_identify(nickservnick, bot_identify);
	if (bot_oper_name && bot_oper_pw)
		ircproto_oper(bot_oper_name, bot_oper_pw);

	while (!want_quit) {
		ircproto_read_message(&prefix, &cmd, &ncmd, &params);
		mutex_on();
		if (ncmd)
			callback_emit_numeric(prefix, ncmd, params);
		else
			callback_emit_str(prefix, cmd, params);

		if (ncmd == RPL_WELCOME) {
			/* distributed on successful user registration */
			for (chans = bot_channels; *chans; chans++)
				ircproto_join(*chans);
		}
		mutex_off();
	}

shutdown:
	mutex_on();
	safe_shutdown();
	return 0;
}
