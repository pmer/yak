#include <string.h>
#include "auth.h"
#include "bool.h"
#include "diagnostic.h"
#include "presence.h"
#include "yak.h"

enum priv modetab[256];
enum priv prefixtab[256];

enum priv mode2priv(char mode)
{
	return modetab[mode];
}

enum priv prefix2priv(char prefix)
{
	return prefixtab[prefix];
}

bool auth_isowner(char *usr)
{
	int i;
	int len;
	char *bang, *owner, **owners;

	/* convert usr to nick */
	bang = strchr(usr, '!');
	len = bang ? bang - usr : strlen(usr);

	for (owners = bot_owners; *owners; owners++) {
		owner = *owners;
		if (!strncmp(usr, owner, len) && strlen(owner) == len)
			return true;
	}
	return false;
}

void auth_set_prefix(char *modes, char *prefixes)
{
	char *mode, *prefix;
	enum priv priv;

	memset(modetab, 0, sizeof(modetab));
	memset(prefixtab, 0, sizeof(modetab));

	priv = PRIV_HIGHEST;
	for (mode = modes, prefix = prefixes; *mode; mode++, prefix++) {
		if (*prefix == '@')
			priv = PRIV_OPERATOR;
		else
			priv--;
		modetab[*mode] = prefixtab[*prefix] = priv;
	}
}

void auth_init()
{
	auth_set_prefix("ov", "@+");
}
