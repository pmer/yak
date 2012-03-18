#ifndef ACL_H
#define ACL_H

/**
 * Acess control lists.
 *
 * Well, more than that. This is a general collection of IRC security functions.
 */

#include "acl.h"
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
 * acl_isowner - is this nick[!user@host] one of my owners?
 */
bool acl_isowner(char *usr);


/**
 * acl_set_prefix - sets the channel modes a user can have
 */
void acl_set_prefix(char *modes, char *prefixes);


/**
 * acl_init - initialize the acl subsystem
 */
void acl_init();

#endif
