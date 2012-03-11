#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bool.h"
#include "callback.h"
#include "ircproto.h"
#include "yak.h"      /* bot_nick */

static void handle_nicknameinuse(char *prefix, int ncmd, char *params)
{
	static bool firsttime = true;
	char *old, *new;

	old = bot_nick;
	new = malloc(strlen(old) + 2);
	sprintf(new, "%s_", old);
	bot_nick = new;

	if (firsttime)
		firsttime = false;
	else
		free(bot_nick);
	bot_nick = new;
	ircproto_nick(bot_nick);
}

int init()
{
	callback_register_numeric(handle_nicknameinuse, ERR_NICKNAMEINUSE);
	return 0;
}

void finish()
{
}

