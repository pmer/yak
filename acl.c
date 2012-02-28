#include <string.h>

#include "acl.h"
#include "yak.h"

bool is_owner_nick(char *usr)
{
	int i;
	int len;
	char *bang, *owner;

	/* convert usr to nick */
	bang = strchr(usr, '!');
	len = bang ? bang - usr : strlen(usr);

	for (i = 0; i < bot_ownercount; i++) {
		owner = bot_owners[i];
		if (!strncmp(usr, owner, len) && strlen(owner) == len)
			return true;
	}
	return false;
}

