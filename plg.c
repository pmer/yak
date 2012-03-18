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
	struct plg *p = malloc(sizeof(struct plg));

	info("loading plugin: %s", plg_name);
	strcpy(p->name, plg_name);
	snprintf(p->file, sizeof(p->file), "plg/%s/%s.so",
		plg_name, plg_name);
	p->so = dlopen(p->file, RTLD_NOW | RTLD_GLOBAL);
	if (pdlerror())
		goto err;
	p->init = dlsym(p->so, "init");
	if (pdlerror())
		goto err;
	p->finish = dlsym(p->so, "finish");
	if (pdlerror())
		goto err;
	if (p->init()) {
		err("plg->init(): failed: %s", plg_name);
		goto err;
	}
	list_add(&p->link, &plgs);
	return;

err:
	free(p);
}

void plg_free_all()
{
	struct list_head *list, *next;
	struct plg *p;

	list_for_each(list, &plgs) {
		p = list_entry(list, struct plg, link);
		p->finish();
		dlclose(p->so);
		pdlerror();
	}
	for (list = &plgs; list != &plgs; list = next) {
		p = list_entry(list, struct plg, link);
		next = list->next;
		free(p);
	}
}
