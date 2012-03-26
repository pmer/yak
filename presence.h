#ifndef PRESENCE_H
#define PRESENCE_H

#include "list.h"

struct chan;
struct user;

/*
 * Commented privileges are unofficial and should be specified in an ISUPPORT
 * message.
 *
 * Confer with plg/isupport/isupport.c and acl_set_prefix in acl.c
 */
enum priv {
	PRIV_NONE,
	/* PRIV_HALF_OP, */
	PRIV_OPERATOR = 10,
	/* PRIV_PROTECTED_OP, */
	/* OWNER, */
	PRIV_HIGHEST = 20,
};

struct presence {
	struct list_head clink, ulink; /* presences in a channel; for a user */
	struct chan *chan;
	struct user *user;
	enum priv priv; /* +v +h +o +a +q */
};

struct presence *presence_new(struct chan *chan, struct user *user);
void presence_del(struct presence *pres);
struct presence *presence_find(struct chan *chan, struct user *user);
struct presence *presence_get(struct chan *chan, struct user *user);
struct presence *presence_get3(struct chan *chan, struct user *user,
	bool *existed);

#endif
