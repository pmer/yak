#include <stdlib.h>
#include <string.h>

#include "bool.h"
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
	if (!strncmp(src, bot_nick, strlen(bot_nick))) {
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
static struct event_numeric_t num_events[MAX_NUMERIC];
static struct hashtab *str_events;

static struct event_str_t privmsg_event[] = {
	{ handle_privmsg, "PRIVMSG" },
};

/*
 * Emitters.
 */
void callback_emit_numeric(char *prefix, int ncmd, char *params)
{
	struct event_numeric_t *e;

	e = &num_events[ncmd];
	if (e->call)
		e->call(prefix, ncmd, params);
}

void callback_emit_str(char *prefix, char *cmd, char *params)
{
	struct event_str_t *e;

	e = hashtab_search(str_events, cmd);
	if (e && e->call)
		e->call(prefix, cmd, params);
}

/*
 * Registration.
 */
void _callback_register_numeric(struct event_numeric_t *events, int size)
{
	struct event_numeric_t *event;
	int i;

	for (i = 0; i < size; i++) {
		event = &events[i];
		if (num_events[event->ncmd].call)
			info("warn: overwriting numeric event %d", event->ncmd);
		num_events[event->ncmd] = *event;
	}
}

void _callback_register_str(struct event_str_t *events, int size)
{
	struct event_str_t *event;
	int i;

	for (i = 0; i < size; i++) {
		event = &events[i];
		if (hashtab_insert(str_events, event->cmd, event) == -1)
			err("err: str event '%s' already exists", event->cmd);
	}
}

/*
 * Struction.
 */
void callback_init()
{
	memset(num_events, 0, sizeof(num_events));
	str_events = hashtab_create(str_hash, strcmp_hash, 15);
	callback_register_str(privmsg_event);
}

