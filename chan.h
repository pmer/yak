#ifndef CHAN_H
#define CHAN_N

#include "list.h"

struct chan {
	struct list_head link;
	char name[512]; /* includes hash prefix */
};

/**
 * chan_init - initialize the channel subsystem
 */
void chan_init();

#endif

