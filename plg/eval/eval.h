#ifndef EVAL_H
#define EVAL_H

#include "bool.h"

typedef void (*eval_callback)(char *line, char **caps, int ncap);

bool eval_emit(char *line);
void eval_register(eval_callback call, char *pattern);

#endif
