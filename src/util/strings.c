#include <string.h>
#include <stdlib.h>

#include "strings.h"

char* duplicate_string(const char* str) {
	/* FIXME: Use preprocessor commands to identify if strdup exists.  */
	size_t len = strlen(str);
	char* result = malloc((len + 1)* sizeof(char));
	if (result != NULL) {
		memcpy(result, str, len + 1);
	}
	return result;
}
