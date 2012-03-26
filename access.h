#ifndef ACCESS_H
#define ACCESS_H

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
 * access_isowner - is this nick[!user@host] one of my owners?
 */
bool access_isowner(char *usr);


/**
 * access_set_prefix - sets the channel modes a user can have
 */
void access_set_prefix(char *modes, char *prefixes);


/**
 * access_init - initialize the access subsystem
 */
void access_init();

#endif
