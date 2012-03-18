#include <stdlib.h>
#include <string.h>
#include "acl.h"
#include "callback.h"
#include "chan.h"
#include "diagnostic.h"
#include "ircproto.h"
#include "presence.h"
#include "usr.h"

static LIST_HEAD(chans);

static char chantypes[20] = "#&";

static void chan_track_start(char *chan_name)
{
	struct chan *chan = malloc(sizeof(struct chan));
	strcpy(chan->name, chan_name);
	INIT_LIST_HEAD(&chan->presences);
	list_add(&chan->link, &chans);
}

static struct chan *chan_find(char *chan_name)
{
	struct list_head *pos;
	struct chan *chan;

	list_for_each(pos, &chans) {
		chan = list_entry(pos, struct chan, link);
		if (!strcmp(chan->name, chan_name))
			return chan;
	}
	return NULL;
}

void chan_set_prefix(char *prefixes)
{
	info("CHANTYPES=%s", prefixes);
	strcpy(chantypes, prefixes);
}

/*
 * Callbacks.
 */
static void handle_join(char *prefix, char *cmd, char *params)
{
	struct chan *chan;
	struct user *user;
	char *chan_name;

	chan_name = strpbrk(params, chantypes);
	if (isme(prefix)) {
		chan_track_start(chan_name);
	}
	else {
		chan = chan_find(chan_name);
		user = user_get(prefix);
		info("user joined: %s in %s", user->nick, chan_name);
		presence_new(chan, user);
	}
}

static void handle_part(char *prefix, char *cmd, char *params)
{
	struct chan *chan;
	struct user *user;
	char *chan_name;

	chan_name = strpbrk(params, chantypes);
	chan = chan_find(chan_name);
	if (chan) {
		user = user_find(prefix);
		info("user left: %s", user->nick);
		presence_del(presence_find(chan, user));
	}
}

static void handle_names(char *prefix, int ncmd, char *params)
{
	struct chan *chan;
	struct user *user;
	struct presence *pres;
	char *chan_name, *space, *nicks, *nick;
	enum priv priv;

	chan_name = strpbrk(params, chantypes);
	if (!chan_name) {
		err("RPL_NAMREPLY: couldn't find %s signifying chan name",
			chantypes);
		return;
	}
	space = strchr(chan_name, ' ');
	nicks = strchr(space, ':') + 1;

	*space = '\0';
	chan = chan_find(chan_name);
	for (nick = strtok(nicks, " "); nick; nick = strtok(NULL, " ")) {
		/* possible list of privs */
		priv = prefix2priv(nick[0]);
		if (priv != PRIV_NONE)
			nick++;
		user = user_get(nick);
		pres = presence_get(chan, user);
		pres->priv = priv;
	}
}

static void handle_names_end(char *prefix, int ncmd, char *params)
{
}

void chan_init()
{
	callback_register_str(handle_join, "JOIN");
	callback_register_str(handle_part, "PART");
	callback_register_numeric(handle_names, RPL_NAMREPLY);
	callback_register_numeric(handle_names_end, RPL_ENDOFNAMES);
}
