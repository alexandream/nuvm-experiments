#include "p-strcasecmp.h"

#include <strings.h>

#ifndef HAS_STRCASECMP
int strcasecmp(const char *left_str, const char *right_str)
{
	const unsigned char* l = (void *)left_str;
	const unsigned char* r = (void *)right_str;

	for (;*l && *r && (*l == *r || tolower(*l) == tolower(*r)); l++, r++);
	return tolower(*l) - tolower(*r);
}
#endif
