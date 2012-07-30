#ifndef EVAL_H
#define EVAL_H

#include <stdbool.h>

struct evalctx;

typedef void (*eval_callback)(char *line, char **caps, int ncap);

struct evalctx *eval_create();
void eval_destroy(struct evalctx *ctx);
bool eval_emit(struct evalctx *ctx, char *line);
void eval_register(struct evalctx *ctx, eval_callback call, char *pattern);

#endif
