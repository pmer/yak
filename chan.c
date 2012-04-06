#include <stdlib.h>
#include <string.h>
#include "access.h"
#include "callback.h"
#include "chan.h"
#include "diagnostic.h"
#include "ircproto.h"
#include "presence.h"
#include "usr.h"

static LIST_HEAD(chans);

static char chantypes[20] = "#&";

static void chan_new(char *chan_name)
{
	struct chan *chan;

	chan = malloc(sizeof(struct chan));
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
		if (!strcasecmp(chan->name, chan_name))
			return chan;
	}
	return NULL;
}

void chan_set_prefix(char *prefixes)
{
	strcpy(chantypes, prefixes);
}

/*
 * Callbacks.
 */
static void handle_join_me(char *chan_name)
{
	chan_new(chan_name);
}

static void handle_join_other(char *chan_name, char *prefix)
{
	struct chan *chan;
	struct user *user;
	struct presence *pres;
	bool existed;

	chan = chan_find(chan_name);
	user = user_get(prefix);
	pres = presence_get3(chan, user, &existed);
	pres->priv = PRIV_NONE;
	if (!existed)
		info("user %s/%s joined", chan_name, user->nick);
	if (existed)
		info("user %s/%s joined: thought already present",
			chan_name, user->nick);
}

static void handle_join(char *prefix, char *cmd, char *params)
{
	char *chan_name;

	chan_name = strpbrk(params, chantypes);
	if (isme(prefix))
		handle_join_me(chan_name);
	else
		handle_join_other(chan_name, prefix);
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
		info("user %s/%s left", chan_name, user->nick);
		presence_del(presence_find(chan, user));
	}
	else {
		info("user %s/%s left: wasn't aware of presence",
			chan_name, prefix);
	}
}

static void handle_quit(char *prefix, char *cmd, char *params)
{
	struct user *user;

	user = user_find(prefix);
	if (user) {
		user_del(user);
		info("user %s quit", user->nick);
	}
	if (!user) {
		/* user->nick doesn't exist */
		info("user %s quit: wasn't aware of user", prefix);
	}
}

static void handle_kick(char *prefix, char *cmd, char *params)
{
	struct chan *chan;
	struct user *user;
	struct presence *pres;
	char *chan_name, *victim_prefix, *save;

	chan_name = strtok_r(params, " ", &save);
	victim_prefix = strtok_r(NULL, " ", &save);
	chan = chan_find(chan_name);
	user = user_find(victim_prefix);
	pres = chan && user ? presence_find(chan, user) : NULL;

	if (pres) {
		info("user %s/%s kicked", chan_name, user->nick);
		presence_del(pres);
	}
	else {
		/* user->nick might not exist */
		info("user %s/%s kicked: wasn't aware of presence",
			chan_name, victim_prefix);
	}
}

static void handle_names(char *prefix, int ncmd, char *params)
{
	struct chan *chan;
	struct user *user;
	struct presence *pres;
	char *chan_name, *space, *nicks, *nick, *save;
	enum priv priv;

	chan_name = strpbrk(params, chantypes);
	space = strchr(chan_name, ' ');
	nicks = strchr(space, ':') + 1;

	*space = '\0';
	chan = chan_find(chan_name);
	for (nick = strtok_r(nicks, " ", &save); nick;
			nick = strtok_r(NULL, " ", &save)) {
		/* XXX: possibly a list of privs */
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
	callback_register_str(handle_quit, "QUIT");
	callback_register_str(handle_kick, "KICK");
	callback_register_numeric(handle_names, RPL_NAMREPLY);
	callback_register_numeric(handle_names_end, RPL_ENDOFNAMES);
}
