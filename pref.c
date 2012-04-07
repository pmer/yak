#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "diagnostic.h"
#include "list.h"
#include "pref.h"
#include "str.h"

struct pref {
	struct list_head link;
	char value[PREF_MAX_LINE_LEN + 1];
};

static LIST_HEAD(prefs);

char *pref_get(char *name)
{
	struct list_head *node;
	struct pref *pref;
	int len;

	len = strlen(name);
	list_for_each(node, &prefs) {
		pref = list_entry(node, struct pref, link);
		if (!strncmp(name, pref->value, len) && pref->value[len] == ' ')
			return pref->value + len + 1; /* skip space */
	}
	return NULL;
}

static char *pref_find_yakrc()
{
	/*
	 * XXX refactor into path.c, rc.c, maybe?
	 * Use for finding plugins.txt and allow for others.
	 */
	static char buf[256];

	strcpy(buf, "./yakrc");
	if (access(buf, R_OK) == 0)
		return buf;

	strcpy(buf, getenv("HOME"));
	strcat(buf, "/.yak/yakrc");
	if (access(buf, R_OK) == 0)
		return buf;

	return NULL;
}

void pref_init()
{
	FILE *f;
	char *filename;
	char line[PREF_MAX_LINE_LEN + 1];
	struct pref *pref;
	int len;

	filename = pref_find_yakrc();
	if (!filename) {
		err("couldn't find yakrc");
		safe_shutdown_and_die(1);
	}
	f = fopen(filename, "r");
	if (!f)
		goto err;
	while (!ferror(f) && !feof(f)) {
		fgets(line, sizeof(line), f);
		chomp(line);
		if (*line) {
			pref = malloc(sizeof(struct pref));
			strcpy(pref->value, line);
			list_add(&pref->link, &prefs);
		}
	}
	if (ferror(f))
		goto err;
	fclose(f);
	add_shutdown_fn(pref_finish);
	return;

err:
	err("error reading %s", filename);
	safe_shutdown_and_die(1);
}

void pref_finish()
{
	struct list_head *node, *next;
	struct pref *pref;

	list_for_each_safe(node, next, &prefs) {
		pref = list_entry(node, struct pref, link);
		free(pref);
	}
}
