#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "callback.h"
#include "diagnostic.h"
#include "hashtab.h"
#include "ircproto.h"
#include "privmsg.h"
#include "yak.h"

/*
 * Callback functions.
 */
static void handle_privmsg(char *usr, char *cmd, char *src_and_msg)
{
	/* src_and_msg is in the following format
	 *
	 * "channel/nick[!user[@host]] :message"
	 */
	char *src, *msg, *spc;
	bool src_on_heap = false;

	spc = strchr(src_and_msg, ' ');
	*spc = '\0';
	src = src_and_msg;
	msg = spc + 2; /* space, colon */
	if (!strncasecmp(src, bot_nick, strlen(bot_nick))) {
		/* Receiving a private query. Strip the source down to just the
		 * username. */
		if (strchr(usr, '!')) {
			src = strdup(usr);
			*strchr(src, '!') = '\0';
			src_on_heap = true;
		}
		else {
			src = usr;
		}
	}

	callback_emit_privmsg(usr, src, msg);

	if (src_on_heap)
		free(src);
}

/*
 * Callback vectors.
 */
static callback_numeric num_events[MAX_NUMERIC];
static struct hashtab *str_events;

/*
 * Emitters.
 */
void callback_emit_numeric(char *prefix, int ncmd, char *params)
{
	callback_numeric call;

	call = num_events[ncmd];
	if (call)
		call(prefix, ncmd, params);
}

void callback_emit_str(char *prefix, char *cmd, char *params)
{
	callback_str call;

	call = hashtab_search(str_events, cmd);
	if (call)
		call(prefix, cmd, params);
}

/*
 * Registration.
 */
void callback_register_numeric(callback_numeric call, int ncmd)
{
	if (num_events[ncmd])
		info("callback_register_numeric: %d: overwriting event", ncmd);
	num_events[ncmd] = call;
}

void callback_register_str(callback_str call, char *cmd)
{
	if (hashtab_insert(str_events, cmd, call) == -1)
		err("callback_register_str: %s: event already exists", cmd);
}

/*
 * Struction.
 */
void callback_init()
{
	memset(num_events, 0, sizeof(num_events));
	str_events = hashtab_create(str_hash, strcmp_hash, 31);
	callback_register_str(handle_privmsg, "PRIVMSG");
}
