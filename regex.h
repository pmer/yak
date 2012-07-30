#ifndef REGEX_H
#define REGEX_H

#include <stdbool.h>

/*
 * Maximum number of captures that can be handled in a regular expression. If
 * this number is too low, regex_match will spontaneously fail with a return 0
 * instead of partially capturing any subpatterns.
 */
#define REGEX_MAX_CAPTURES 10

enum regex_compiled_state {
	RCS_UNINIT,
	RCS_ERROR,
	RCS_COMPILED,
};

struct regex {
	char *pattern;
	enum regex_compiled_state state;
	void *re_detail; /* struct pcre* re_detail */
};

/**
 * regex_match - match with lazy compile
 * @re    regular expression to be used for matching
 * @str   string to match against
 * @caps  array of pre-allocated capture buffers or NULL.
 *
 * Compiles the regular expression if necessary and runs a match against a
 * string.
 *
 * If caps is NULL, captures are not saved. Otherwise, captures matched by the
 * regular expression are strcpy()'d into caps[0], caps[1], etc.
 *
 * Returns 1 + the number of successful captures if the string matches the
 * regex, or a negative number if it does not.
 */
int regex_match(struct regex *re, char *str, char **caps);

/**
 * regex_free - tries to free a compiled regex
 * @re  regular expression to free
 *
 * Frees the data associated with a compiled regular expression if it has any.
 * This function is safe to run even on uncompiled regexes.
 */
void regex_free(struct regex *re);

#endif
