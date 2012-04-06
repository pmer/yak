#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bool.h"
#include "diagnostic.h"
#include "list.h"
#include "plg.h"

struct plg {
	struct list_head link;
	char name[256];
	char file[256];
	void *so;
	int (*init)();
	void (*finish)();
};

/*
 * List of loaded plugins.
 */
static LIST_HEAD(plgs);


/**
 * pdlerror - print dlerror
 */
static bool pdlerror()
{
	char *errmsg = dlerror();
	if (errmsg)
		err("dlerror: %s", errmsg);
	return errmsg != NULL;
}


void plg_load_plgs_from(char *filename)
{
	FILE *f = fopen(filename, "r");
	int len;
	char line[256];

	if (!f) {
		err("plg_load_plgs_in: cannot open %s", filename);
		return;
	}
	while (fgets(line, sizeof(line), f)) {
		len = strlen(line);
		line[len-- - 1] = '\0'; /* chop trailing '\n' */
		if (len && line[0] != '#')
			plg_load(line);
	}
	fclose(f);

	add_shutdown_fn(plg_free_all);
}

void plg_load(char *plg_name)
{
	struct plg *plg = malloc(sizeof(struct plg));

	strncpy(plg->name, plg_name, sizeof(plg->name));
	snprintf(plg->file, sizeof(plg->file), "plg/%s/%s.so",
		plg_name, plg_name);
	plg->so = dlopen(plg->file, RTLD_NOW | RTLD_GLOBAL);
	if (pdlerror())
		goto err;
	plg->init = dlsym(plg->so, "init");
	if (pdlerror())
		goto err;
	plg->finish = dlsym(plg->so, "finish");
	if (pdlerror())
		goto err;
	if (plg->init()) {
		err("plg->init(): failed: %s", plg_name);
		goto err;
	}
	list_add(&plg->link, &plgs);
	return;

err:
	free(plg);
}

static struct plg *plg_find(char *plg_name)
{
	struct list_head *list;
	struct plg *plg;

	list_for_each(list, &plgs) {
		plg = list_entry(list, struct plg, link);
		if (!strncmp(plg_name, plg->name, sizeof(plg->name)))
			return plg;
	}
	err("plg_find: %s: not found", plg_name);
	return NULL;
}

void *plg_sym(char *plg_name, char *symbol)
{
	struct plg *plg;
	void *addr;

	plg = plg_find(plg_name);
	if (plg) {
		addr = dlsym(plg->so, symbol);
		pdlerror();
		return addr;
	}
	else {
		return NULL;
	}
}

void plg_free_all()
{
	struct list_head *list, *next;
	struct plg *plg;

	list_for_each_safe(list, next, &plgs) {
		plg = list_entry(list, struct plg, link);
		plg->finish();
		dlclose(plg->so);
		pdlerror();
		free(plg);
	}
}
