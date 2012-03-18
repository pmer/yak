#ifndef CHAN_H
#define CHAN_N

#include "list.h"

struct chan {
	struct list_head link;
	struct list_head presences;
	char name[512]; /* includes channel prefix */
};

void chan_set_prefix(char *prefixes);

/**
 * chan_init - initialize the channel subsystem
 */
void chan_init();

#endif
