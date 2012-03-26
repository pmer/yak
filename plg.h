#ifndef PLG_H
#define PLG_H

/**
 * plg_load_plgs_from - load plugins listed one per line in a text file
 */
void plg_load_plgs_from(char *filename);

/**
 * plg_load - load a Yak plugin from 'plg/$name/$name.so'
 */
void plg_load(char *plg_name);

/**
 * plg_sym - try to find a function in a plugin
 *
 * @returns pointer to function if found,
 *          NULL if plugin not loaded or if symbol not found in plugin
 */
void *plg_sym(char *plg_name, char *symbol);

/**
 * plg_free_all - free all plugins safely, running finish functions on each
 */
void plg_free_all();

#endif

