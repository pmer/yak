#include <stdlib.h>
#include "callback.h"
#include "ircproto.h"
#include "list.h"
#include "loggedin.h"

bool loggedin = false;

struct callnode
{
	struct list_head link;
	fn call;
};

static LIST_HEAD(calls);

void onlogin(fn call)
{
	struct callnode *n = malloc(sizeof(struct callnode));
	n->call = call;
	list_add(&n->link, &calls);
}

static void login(char *prefix, int ncmd, char *params)
{
	struct list_head *pos, *next;
	struct callnode *node;

	loggedin = true;
	list_for_each_safe(pos, next, &calls) {
		node = list_entry(pos, struct callnode, link);
		node->call();
		list_del(pos);
		free(node);
	}
}

int init()
{
	callback_register_numeric(login, RPL_LOGGEDIN);
	return 0;
}

void finish()
{
}
