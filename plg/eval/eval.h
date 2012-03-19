#ifndef EVAL_H
#define EVAL_H

typedef void (*eval_callback)(char *line, char **caps, int ncap);

void eval_emit(char *line);
void eval_register(eval_callback call, char *pattern);

#endif
