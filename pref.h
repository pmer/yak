#ifndef PREF_H
#define PREF_H

#define PREF_MAX_LINE_LEN 1024

char *pref_get(char *name);

void pref_init();
void pref_finish();

#endif
