#ifndef __NUVM__VALUE_H__
#define __NUVM__VALUE_H__

#include "util/common.h"


#define HALF_TAG_POINTER   0x0000
#define HALF_TAG_IMMEDIATE 0xFFFF
#define TAG_FIXNUM         0xFFFFFFFFU
#define FLONUM_ADJUST      0x0001000000000000UL

#define POINTER_MASK       0xFFFFFFFFFFFFFFF8UL

uint32_t NUVM_FIXNUM_T_TYPE();
uint32_t NUVM_FLONUM_T_TYPE();
uint32_t NUVM_BOOLEAN_T_TYPE();

typedef uint32_t tag_t;
typedef uint16_t half_tag_t;

// FIXME: This structure assumes the machine under which this is being compiled
// uses a little endian byte order (and a certain specific set of alignment
// requirements). This is seen in the order of the lower and upper fields, and
// the order of the two union fields of the composite struct.  In order to make
// this portable, quite a bit of conditional compilation logic (or a completely
// different value_t representation swapped in at compile time) would be
// needed.


typedef union nuvm_value_t {
	uint64_t contents;
	void*    pointer;
	double   flonum;

	struct {
		int32_t fixnum;
		union {
			tag_t tag;
			struct {
				half_tag_t lower_tag;
				half_tag_t upper_tag;
			};
		};
	} immediate;
} nuvm_value_t;

typedef struct nuvm_object_t {
	uint32_t type_id;
} nuvm_object_t;


extern nuvm_value_t NUVM_TRUE, NUVM_FALSE;

static inline
nuvm_value_t nuvm_wrap_pointer(void* pointer) {
	nuvm_value_t value;
	value.pointer = pointer;
	return value;
}

static inline
bool nuvm_is_pointer(nuvm_value_t value) {
	return value.immediate.upper_tag == HALF_TAG_POINTER;
}

static inline
void* nuvm_unwrap_pointer(nuvm_value_t value) {
	return (void*) ((uint64_t) value.pointer);
}

static inline
nuvm_value_t nuvm_wrap_fixnum(int32_t fixnum) {
	nuvm_value_t result;
	result.immediate.tag = TAG_FIXNUM;
	result.immediate.fixnum = fixnum;
	return result;
}

static inline
int32_t nuvm_unwrap_fixnum(nuvm_value_t value) {
	return value.immediate.fixnum;
}

static inline
bool nuvm_is_immediate(nuvm_value_t value) {
	return !nuvm_is_pointer(value);
}

static inline
bool nuvm_is_flonum(nuvm_value_t value) {
	return nuvm_is_immediate(value) &&
	       value.immediate.upper_tag != HALF_TAG_IMMEDIATE;
}

static inline
bool nuvm_is_fixnum(nuvm_value_t value) {
	return value.immediate.tag == TAG_FIXNUM;
}


static inline
bool nuvm_is_equal(nuvm_value_t v1, nuvm_value_t v2) {
	return v1.contents == v2.contents;
}

static inline
nuvm_value_t nuvm_wrap_flonum(double flonum) {
	nuvm_value_t result;
	result.flonum = flonum;
	result.contents += FLONUM_ADJUST;
	return result;
}

static inline
double nuvm_unwrap_flonum(nuvm_value_t value) {
	value.contents -= FLONUM_ADJUST;
	return value.flonum;
}

static inline
bool nuvm_is_boolean(nuvm_value_t value) {
	return nuvm_is_equal(value, NUVM_TRUE) ||
		   nuvm_is_equal(value, NUVM_FALSE);
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
	else if (nuvm_is_flonum(value)) {
		type_id = NUVM_FLONUM_T_TYPE();
	}
	else if (nuvm_is_fixnum(value)) {
		type_id = NUVM_FIXNUM_T_TYPE();
	}
	else if (nuvm_is_boolean(value)) {
		type_id = NUVM_BOOLEAN_T_TYPE();
	}
	return type_id;
}

void nuvm_value_t_init();
#undef HALF_TAG_POINTER
#undef HALF_TAG_IMMEDIATE
#undef TAG_FIXNUM
#undef FLONUM_ADJUST

#undef POINTER_MASK

#endif

