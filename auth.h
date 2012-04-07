#ifndef AUTH_H
#define AUTH_H

/**
 * Acess control.
 */

#include "bool.h"

/**
 * mode2priv - convert a channel mode to a user privilege level
 */
enum priv mode2priv(char mode);

/**
 * prefix2priv - convert a nickname prefix to a user privilege level
 */
enum priv prefix2priv(char prefix);


/**
 * auth_isowner - is this nick[!user@host] one of my owners?
 */
bool auth_isowner(char *usr);


/**
 * auth_set_prefix - sets the channel modes a user can have
 */
void auth_set_prefix(char *modes, char *prefixes);


/**
 * auth_init - initialize the auth subsystem
 */
void auth_init();

#endif
