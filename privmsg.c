#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "callback.h"
#include "diagnostic.h"
#include "hashtab.h"
#include "ircproto.h"
#include "list.h"
#include "plg.h"
#include "privmsg.h"
#include "yak.h"

/*
 * Callback data structure.
 */
struct re_event {
	struct list_head link;
	callback_privmsg_re call;
	struct regex match;
};

/**
 * str_events - list of string that we will listen for
 */
static struct hashtab *str_events;

/**
 * re_events - list of regular expression events that we will listen for
 */
static LIST_HEAD(re_events);


/*
 * Emitters.
 */
void callback_emit_privmsg(char *usr, char *src, char *msg)
{
	callback_privmsg_str call;
	struct list_head *node;
	struct re_event *rev;
	int matches, ncap, i;

	static char *caps[REGEX_MAX_CAPTURES];
	static char caps_buf[512 * REGEX_MAX_CAPTURES];
	static bool need_init = true;

	if (need_init) {
		/* setup static memory for regex captures */
		need_init = false;
		for (i = 0; i < REGEX_MAX_CAPTURES; i++)
			caps[i] = &caps_buf[512 * i];
	}

	/* string events */
	call = hashtab_search(str_events, msg);
	if (call)
		call(usr, src, msg);

	/* regex events */
	list_for_each(node, &re_events) {
		rev = list_entry(node, struct re_event, link);
		matches = regex_match(&rev->match, msg, caps);
		ncap = matches - 1;
		if (ncap >= 0)
			rev->call(usr, src, msg, caps, ncap);
	}
}

void callback_register_privmsg_str(callback_privmsg_str call, char *str)
{
	if (hashtab_insert(str_events, str, call) == -1)
		err("str event '%s' already exists", str);
}

void callback_register_privmsg_re(callback_privmsg_re call, char *pattern)
{
	struct re_event *e = malloc(sizeof(struct re_event));
	e->call = call;
	e->match.pattern = pattern;
	e->match.state = RCS_UNINIT;
	list_add(&e->link, &re_events);
}


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
 * (Con|De)struction
 */
static void free_str_events()
{
	hashtab_destroy(str_events);
}

void privmsg_init()
{
	str_events = hashtab_create(str_hash, strcmp_hash,
		PRIVMSG_CSTR_BUCKETS);
	callback_register_str(handle_privmsg, "PRIVMSG");
	add_shutdown_fn(free_str_events);
}
