#include <assert.h>
#include <stdbool.h>
#include <string.h>
#include "callback.h"
#include "diagnostic.h"
#include "hashtab.h"

/*
 * Callback vectors.
 */
static callback_numeric num_events[MAX_NUMERIC];
static struct hashtab *str_events;

/*
 * Emitters.
 */
void callback_emit_numeric(char *prefix, int ncmd, char *params)
{
	callback_numeric call;

	if (ncmd >= MAX_NUMERIC) {
		info("irc numeric of %d greater than MAX_NUMERIC (%d)",
				ncmd, MAX_NUMERIC);
		return;
	}

	call = num_events[ncmd];
	if (call)
		call(prefix, ncmd, params);
}

void callback_emit_str(char *prefix, char *cmd, char *params)
{
	callback_str call;

	call = hashtab_search(str_events, cmd);
	if (call)
		call(prefix, cmd, params);
}

/*
 * Registration.
 */
void callback_register_numeric(callback_numeric call, int ncmd)
{
	assert(ncmd < MAX_NUMERIC);

	if (num_events[ncmd])
		info("callback_register_numeric: %d: overwriting event", ncmd);
	num_events[ncmd] = call;
}

void callback_register_str(callback_str call, char *cmd)
{
	if (hashtab_insert(str_events, cmd, call) == -1)
		err("callback_register_str: %s: event already exists", cmd);
}

/*
 * (Con|De)struction.
 */
static void free_str_events()
{
	hashtab_destroy(str_events);
}

void callback_init()
{
	memset(num_events, 0, sizeof(num_events));
	str_events = hashtab_create(str_hash, strcmp_hash, 31);
	add_shutdown_fn(free_str_events);
}
