#ifndef __NUVM__VALUE_H__
#define __NUVM__VALUE_H__

#include "util/types.h"


typedef union {
	uint64_t contents;
	void* pointer;
} NValue;

typedef struct {
	int32_t type_id;
} NObject;


NValue n_wrap_pointer(void* pointer);
void*  n_unwrap_pointer(NValue value);
bool   n_is_pointer(NValue value);

bool n_is_equal(NValue value1, NValue value2);

int32_t n_typeof(NValue value);
#endif
