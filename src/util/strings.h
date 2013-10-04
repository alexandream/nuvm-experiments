#ifndef __NUVM__UTIL__STRINGS_H__
#define __NUVM__UTIL__STRINGS_H__

#include <string.h>
#include <stdlib.h>

static inline
char* duplicate_string(const char* str) {
	size_t len = strlen(str);
	// FIXME: Check return value for errors.
	char* result = malloc(len * sizeof(char));
	memcpy(result, str, len * sizeof(char));
	return result;
}

#endif
