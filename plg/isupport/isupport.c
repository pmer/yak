/*
 * IRC ISUPPORT support
 *
 * IRC servers can advertise non-standard protocol extensions with the ISUPPORT
 * message when one first logs in.
 *
 * Documentation:
 *   www.irc.org/tech_docs/draft-brocklesby-irc-isupport-03.txt
 */

#include <string.h>
#include "bool.h"
#include "callback.h"
#include "ircproto.h"

void isupport_set(char *param, char *value)
{
	char *modes, *prefixes, *paren;

	if (!strcmp(param, "CHANTYPES")) {
		chan_set_prefix(value);
	}
	else if (!strcmp(param, "PREFIX")) {
		if (value) {
			paren = strchr(value, ')');
			*paren = '\0';
			modes = value + 1;
			prefixes = paren + 1;
			acl_set_prefix(modes, prefixes);
		}
		else {
			acl_set_prefix("", "");
		}
	}
}

void isupport_unset(char *param)
{
	if (!strcmp(param, "CHANTYPES"))
		chan_set_prefix("#&");
	else if (!strcmp(param, "PREFIX"))
		acl_set_prefix("ov", "@+");
}

void isupport_token(char *tok)
{
	char *param = tok, *value = NULL, *equals;
	bool set;

	set = tok[0] != '-';
	if (tok[0] == '-')
		tok++;

	equals = strchr(param, '=');
	if (equals) {
		*equals = '\0';
		value = equals + 1;
	}
	if (value && *value == '\0')
		value = NULL;

	if (set)
		isupport_set(param, value);
	else
		isupport_unset(param);
}

void handle_isupport(char *prefix, int ncmd, char *params)
{
	char *tok, *save;

	tok = strtok_r(params, " ", &save);
	/* ignore the first token, it is our nickname */

	while ((tok = strtok_r(NULL, " ", &save)) && tok[0] != ':')
		isupport_token(tok);
}

int init()
{
	callback_register_numeric(handle_isupport, RPL_ISUPPORT);
	return 0;
}

void finish()
{
}
