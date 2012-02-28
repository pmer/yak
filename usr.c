#include <string.h>

#include "usr.h"
#include "yak.h"

bool ismynick(char *usr)
{
	/* To verify identity only have to test nickname, that is enough. No
	 * need to look at username or host. */
	char *bang;
	int len;

	bang = strchr(usr, '!');
	if (bang) {
		len = bang - usr;
		return !strncmp(usr, bot_nick, len) &&
			strlen(bot_nick) == len;
	}
	else {
		return !strcmp(usr, bot_nick);
	}
}

