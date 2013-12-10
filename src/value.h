#ifndef __NUVM__VALUE_H__
#define __NUVM__VALUE_H__

#include "util/types.h"


typedef union {
	uint64_t contents;
	void* pointer;
} NValue;

NValue n_wrap_pointer(void* pointer);
void*  n_unwrap_pointer(NValue value);
bool   n_is_pointer(NValue value);

bool n_is_equal(NValue value1, NValue value2);
#endif
