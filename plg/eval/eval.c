#include <stdlib.h>
#include "eval.h"
#include "list.h"
#include "regex.h"

struct re_event {
	struct list_head link;
	eval_callback call;
	struct regex match;
};

/**
 * events - list of regular expression events that we will listen for
 */
static LIST_HEAD(events);


void eval_emit(char *line)
{
	eval_callback call;
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

	list_for_each(node, &events) {
		rev = list_entry(node, struct re_event, link);
		matches = regex_match(&rev->match, line, caps);
		ncap = matches - 1;
		if (ncap >= 0)
			rev->call(line, caps, ncap);
	}
}

void eval_register(eval_callback call, char *pattern)
{
	struct re_event *e = malloc(sizeof(struct re_event));
	e->call = call;
	e->match.pattern = pattern;
	e->match.state = RCS_UNINIT;
	list_add(&e->link, &events);
}

void init()
{
}

void finish()
{
}
