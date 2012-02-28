#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bool.h"
#include "diagnostic.h"
#include "list.h"
#include "plg.h"

struct plg_t {
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
static struct plg_t plgs = {
	.link = LIST_HEAD_INIT(plgs.link),
};


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
	char line[256];

	if (!f) {
		err("plg_load_plgs_in: cannot open %s", filename);
		return;
	}
	while (fgets(line, sizeof(line), f)) {
		line[strlen(line) - 1] = '\0'; /* rm trailing '\n' */
		plg_load(line);
	}
	fclose(f);

	add_shutdown_fn(plg_free_all);
}

void plg_load(char *plg_name)
{
	struct plg_t *plg = malloc(sizeof(struct plg_t));

	info("loading plugin: %s", plg_name);
	strcpy(plg->name, plg_name);
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
	list_add(&plg->link, &plgs.link);
	return;

err:
	free(plg);
}

void plg_free_all()
{
	struct list_head *list, *next;
	struct plg_t *plg;

	list_for_each(list, &plgs.link) {
		plg = list_entry(list, struct plg_t, link);
		plg->finish();
		dlclose(plg->so);
		pdlerror();
	}
	for (list = &plgs.link; list != &plgs.link; list = next) {
		plg = list_entry(list, struct plg_t, link);
		next = list->next;
		free(plg);
	}
}

