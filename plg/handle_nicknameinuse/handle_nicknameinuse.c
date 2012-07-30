#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "callback.h"
#include "ircproto.h"
#include "yak.h"

static void handle_nicknameinuse(char *prefix, int ncmd, char *params)
{
	size_t len = strlen(bot_nick);
	bot_nick = realloc(bot_nick, len + 2);
	bot_nick[len++] = '_';
	bot_nick[len] = '\0';
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
