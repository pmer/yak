#include <stdlib.h>
#include "privmsg.h"
#include "ctcp.h"
#include "list.h"
#include "regex.h"

struct re_event {
	struct list_head link;
	ctcp_callback call;
	struct regex match;
};


/**
 * events - list of regular expression events that we will listen for
 */
static LIST_HEAD(events);


void ctcp_emit(char *usr, char *src, char *msg)
{
	ctcp_callback call;
	struct list_head *list;
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

	/* regex events */
	list_for_each(list, &events) {
		rev = list_entry(list, struct re_event, link);
		matches = regex_match(&rev->match, msg, caps);
		ncap = matches - 1;
		if (ncap >= 0)
			rev->call(usr, src, msg, caps, ncap);
	}
}

void ctcp_register(ctcp_callback call, char *pattern)
{
	struct re_event *e = malloc(sizeof(struct re_event));
	e->call = call;
	e->match.pattern = pattern;
	e->match.state = RCS_UNINIT;
	list_add(&e->link, &events);
}

static void handle_ctcp(char *usr, char *src, char *msg,
		char **caps, int ncaps)
{
	char *message;
	
	message = caps[0];
	
	ctcp_emit(usr, src, message);
}

int init()
{
	callback_register_privmsg_re(handle_ctcp, "^\001(.*)\001$");
	return 0;
}

void finish()
{
}

