#include <stdlib.h>
#include "eval.h"
#include "list.h"
#include "regex.h"

struct evalctx {
	/**
	 * events - list of regular expression events that we will listen for
	 */
	struct list_head events;
};

struct re_event {
	struct list_head link;
	eval_callback call;
	struct regex match;
};


struct evalctx *eval_create()
{
	struct evalctx *ctx;

	ctx = malloc(sizeof(struct evalctx));
	INIT_LIST_HEAD(&ctx->events);
	return ctx;
}

void eval_destroy(struct evalctx *ctx)
{
	struct list_head *node, *next;
	struct re_event *e;

	list_for_each_safe(node, next, &ctx->events) {
		e = list_entry(node, struct re_event, link);
		list_del(node);
		free(e);
	}
	free(ctx);
}

bool eval_emit(struct evalctx *ctx, char *line)
{
	eval_callback call;
	struct list_head *node;
	struct re_event *rev;
	int matches, ncap, i;
	bool called = false;

	static char *caps[REGEX_MAX_CAPTURES];
	static char caps_buf[512 * REGEX_MAX_CAPTURES];
	static bool need_init = true;

	if (need_init) {
		/* setup static memory for regex captures */
		need_init = false;
		for (i = 0; i < REGEX_MAX_CAPTURES; i++)
			caps[i] = &caps_buf[512 * i];
	}

	list_for_each(node, &ctx->events) {
		rev = list_entry(node, struct re_event, link);
		matches = regex_match(&rev->match, line, caps);
		ncap = matches - 1;
		if (ncap >= 0) {
			rev->call(line, caps, ncap);
			called = true;
		}
	}

	return called;
}

void eval_register(struct evalctx *ctx, eval_callback call, char *pattern)
{
	struct re_event *e = malloc(sizeof(struct re_event));
	e->call = call;
	e->match.pattern = pattern;
	e->match.state = RCS_UNINIT;
	list_add(&e->link, &ctx->events);
}

void init()
{
}

void finish()
{
}
