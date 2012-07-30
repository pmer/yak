#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "diagnostic.h"
#include "hashtab.h"
#include "ircproto.h"
#include "list.h"
#include "plg.h"
#include "privmsg.h"

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

void privmsg_init()
{
	str_events = hashtab_create(str_hash, strcmp_hash,
		PRIVMSG_CSTR_BUCKETS);
}
