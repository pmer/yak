#ifndef USR_H
#define USR_H

#include "bool.h"
#include "list.h"

struct user {
	char nick[512];
	char host[512];
	char uname[512];
	char rname[512];
	struct list_head presences;
};

/**
 * isme - is this IRC prefix me?
 *
 * @prefix - IRC message prefix, in the format of:
 *           "nickname[!username[@host]]"
 */
bool isme(char *prefix);

/**
 * user_del - delete a struct user
 */
void user_del(struct user *user);

/**
 * user_find - find a preexisting struct user
 *
 * @prefix - IRC message prefix, in the format of:
 *           "nickname[!username[@host]]"
 */
struct user *user_find(char *prefix);

/**
 * user_get - find preexisting or create new struct user
 *
 * @prefix - IRC message prefix, in the format of:
 *           "nickname[!username[@host]]"
 */
struct user *user_get(char *prefix);

/**
 * usr_init - initialize the user subsystem
 */
void usr_init();

#endif
