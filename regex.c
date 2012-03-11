#include <pcre.h>
#include <string.h>

#include "diagnostic.h"
#include "regex.h"

/*
 * Complete PCRE documentation:
 * http://www.pcre.org/pcre.txt
 */

#define REGEX_MATCH_VEC_SZ (3*(REGEX_MAX_CAPTURES+1))

static void regex_compile(struct regex *re)
{
	const char* err;
	int erroff;

	/* PCRE_CASELESS */
	re->re_detail = pcre_compile(re->pattern, 0, &err, &erroff, NULL);
	if (re->re_detail) {
		re->state = RCS_COMPILED;
	}
	else {
		re->state = RCS_ERROR;
		info("regex '%s':%d: %s", re->pattern, erroff, err);
	}
}

static int regex_match_exec(struct regex *re, char *str, char **caps)
{
	/* assumes re->state == RCS_COMPILED */
	int vec[REGEX_MATCH_VEC_SZ];
	int matches, beg, end, len, i;
	char* cap;

	matches = pcre_exec(re->re_detail, NULL, str, strlen(str),
		0, 0, vec, REGEX_MATCH_VEC_SZ);
	if (matches < PCRE_ERROR_NOMATCH) {
		/* Something's up... */
		info("pcre_exec failed: code=%d, re=%s, str=%s",
			matches, re->pattern, str);
	}
	if (caps) {
		for (i = 0; i < matches - 1; i++) {
			cap = caps[i];
			beg = vec[2*i + 2]; /* match byte offset into str */
			end = vec[2*i + 3]; /* end of match + 1 */
			len = end - beg;

			memcpy(cap, &str[beg], len);
			cap[len] = '\0';
		}
	}
	return matches;
}

int regex_match(struct regex *re, char *str, char **caps)
{
	switch (re->state) {
	case RCS_UNINIT:
		regex_compile(re);
		return regex_match(re, str, caps);
	case RCS_COMPILED:
		return regex_match_exec(re, str, caps);
	case RCS_ERROR:
		return 0;
	}

	return 0;
}

void regex_free(struct regex *re)
{
	if (re->state == RCS_COMPILED)
		pcre_free(re->re_detail);
}

