#ifndef __NUVM__TYPES__STRING_I_H__
#define __NUVM__TYPES__STRING_I_H__

#include "strings.h"

struct nuvm_string_t {
	nuvm_object_t parent;
	const char* string;
	uint32_t size;
};


#endif
