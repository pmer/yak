#include <stdlib.h>
#include <string.h>

#include "callback.h"
#include "chan.h"
#include "usr.h"

static struct chan_t chans = {
	.link = LIST_HEAD_INIT(chans.link),
};

static void chan_track_start(char *chan_name)
{
	struct chan_t *chan = malloc(sizeof(struct chan_t));
	strcpy(chan->name, chan_name);
	list_add(&chan->link, &chans.link);
}



/*
 * Callbacks.
 */
static void handle_join(char *prefix, char *cmd, char *params)
{
	char *usr = prefix, *chan_name;

	if (ismynick(usr)) {
		chan_name = strchr(params, '#');
		chan_track_start(chan_name);
	}
}

static struct event_str_t str_events[] = {
	{ handle_join, "JOIN" },
};

void chan_init()
{
	callback_register_str(str_events);
}

