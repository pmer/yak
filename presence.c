#include <stdlib.h>
#include "chan.h"
#include "diagnostic.h"
#include "link.h"
#include "presence.h"
#include "usr.h"

struct presence *presence_new(struct chan *chan, struct user *user)
{
	struct presence *pres;

	pres = malloc(sizeof(struct presence));
	pres->chan = chan;
	pres->user = user;
	pres->priv = PRIV_NONE;
	list_add(&pres->clink, &chan->presences);
	list_add(&pres->ulink, &user->presences);

	return pres;
}

void presence_del(struct presence *pres)
{
	struct user *user = pres->user;

	list_del(&pres->clink);
	list_del(&pres->ulink);
	free(pres);
	if (list_empty(&user->presences))
		user_del(user);
}

struct presence *presence_find(struct chan *chan, struct user *user)
{
	struct list_head *node;
	struct presence *pres;

	list_for_each(node, &user->presences) {
		pres = list_entry(node, struct presence, ulink);
		if (pres->chan == chan)
			return pres;
	}
	return NULL;
}

struct presence *presence_get(struct chan *chan, struct user *user)
{
	struct presence *pres;

	pres = presence_find(chan, user);
	return pres ? pres : presence_new(chan, user);
}

struct presence *presence_get3(struct chan *chan, struct user *user,
	bool *existed)
{
	struct presence *pres;

	pres = presence_find(chan, user);
	*existed = pres != NULL;
	return pres ? pres : presence_new(chan, user);
}
