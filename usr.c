#include <stdlib.h>
#include <string.h>
#include "diagnostic.h"
#include "hashtab.h"
#include "ircproto.h"
#include "presence.h"
#include "usr.h"
#include "yak.h"

static struct hashtab *users;

bool isme(char *prefix)
{
	/* To verify identity only have to test nickname, that is enough. No
	 * need to look at username or host. */
	char *bang;
	int len;

	bang = strchr(prefix, '!');
	if (bang) {
		len = bang - prefix;
		return !strncmp(prefix, bot_nick, len) &&
			strlen(bot_nick) == len;
	}
	else {
		return !strcmp(prefix, bot_nick);
	}
}

/**
 * user_new - creates a new struct user object
 *
 * The nickname, username, and host are parsed from the specified prefix, if
 * possible.
 *
 * @prefix - IRC message prefix, in the format of:
 *           "nickname[!username[@host]]"
 */
static struct user *user_new(char *prefix)
{
	struct user *user;

	user = malloc(sizeof(struct user));
	ircproto_parse_prefix(prefix,
		user->nick,  sizeof(user->nick),
		user->uname, sizeof(user->uname),
		user->host,  sizeof(user->host));
	user->rname[0] = '\0';
	INIT_LIST_HEAD(&user->presences);
	info("user_new: %s", user->nick);
	if (hashtab_insert(users, user->nick, user) == -1)
		err("user_new: %s: user already exists", user->nick);
	return user;
}

void user_del(struct user *user)
{
	hashtab_remove(users, user->nick);
	free(user);
}

struct user *user_find(char *prefix)
{
	struct user *user;
	char nick[512];

	ircproto_parse_prefix(prefix, nick, sizeof(nick), NULL, 0, NULL, 0);
	user = hashtab_search(users, nick);
	return user;
}

struct user *user_get(char *prefix)
{
	struct user *user;

	user = user_find(prefix);
	return user ? user : user_new(prefix);
}

void usr_init()
{
	users = hashtab_create(str_hash, strcmp_hash, 101);
}
