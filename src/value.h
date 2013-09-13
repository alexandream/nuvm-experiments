#ifndef __NUVM__VALUE_H__
#define __NUVM__VALUE_H__

#include "util/common.h"


typedef union nuvm_value_t {
	uint64_t contents;
	void*    pointer;
} nuvm_value_t;

typedef struct nuvm_object_t {
	uint32_t type_id;
} nuvm_object_t;



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

static inline
nuvm_value_t nuvm_object_as_value(nuvm_object_t* self) {
	return nuvm_wrap_pointer(self);
}

static inline
void nuvm_construct_object(nuvm_object_t* self, uint32_t type_id) {
	self->type_id = type_id;
}

static inline
uint32_t nuvm_typeof(nuvm_value_t value) {
	uint32_t type_id = 0;
	if (nuvm_is_pointer(value)) {
		nuvm_object_t* obj = (nuvm_object_t*) nuvm_unwrap_pointer(value);
		type_id = obj->type_id;
	}
	return type_id;
}


#endif
