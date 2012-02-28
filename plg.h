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
 * plg_free_all - free all plugins safely, running finish functions on each
 */
void plg_free_all();

#endif

