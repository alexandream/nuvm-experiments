#ifndef __NUVM__VALUE_H__
#define __NUVM__VALUE_H__

#include "util/common.h"


typedef union nuvm_value_t {
	uint64_t contents;
	void*    pointer;
} nuvm_value_t;


static inline
nuvm_value_t nuvm_wrap_pointer(void* pointer) {
	nuvm_value_t value;
	value.pointer = pointer;
	return value;
}

static inline
bool nuvm_is_pointer(nuvm_value_t value) {
	return true;
}

static inline
void* nuvm_unwrap_pointer(nuvm_value_t value) {
	return value.pointer;
}

static inline
bool nuvm_is_equal(nuvm_value_t v1, nuvm_value_t v2) {
	return v1.contents == v2.contents;
}
#endif
