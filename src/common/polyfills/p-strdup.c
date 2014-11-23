#include <string.h>
#include <stdlib.h>

#ifndef HAVE_STRDUP
char *strdup(const char *str)
{
	size_t length = strlen(str);
	char *result = malloc(length + 1);
	if (!result) {
		return NULL;
	}
	memcpy(result, str, length + 1);
	return result;
}
#endif
