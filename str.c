#include <string.h>
#include "str.h"

char *chomp(char *str)
{
	int last = strlen(str) - 1;
	if (str[last] == '\n')
		str[last] = '\0';
	return str;
}
