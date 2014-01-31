#ifndef __NUVM__VALUE_H__
#define __NUVM__VALUE_H__

#include "util/types.h"


typedef union {
	uint64_t contents;
	void* pointer;
	struct {
		union {
			int32_t fixnum;
			int32_t symbol_id;
		} data;
		union {
			uint32_t full_tag;
			struct {
				uint16_t lower_tag;
				uint16_t upper_tag;
			} half_tags;
		} tags;
	} immediate;
} NValue;

extern NValue N_TRUE;
extern NValue N_FALSE;
extern NValue N_UNDEFINED;

typedef struct {
	int32_t type_id;
} NObject;

void
n_init_values();

NValue
n_wrap_fixnum(int32_t fixnum);

NValue
n_wrap_pointer(void* pointer);

NValue
n_wrap_symbol(int32_t symbol_id);

int32_t
n_unwrap_fixnum(NValue value);

void*
n_unwrap_pointer(NValue value);

int32_t
n_unwrap_symbol(NValue value);

bool
n_is_boolean(NValue value);

bool
n_is_fixnum(NValue value);

bool
n_is_pointer(NValue value);

bool
n_is_symbol(NValue value);

bool
n_is_undefined(NValue value);

bool
n_is_equal(NValue value1, NValue value2);

NValue
n_to_string(NValue value);

int32_t
n_typeof(NValue value);
#endif
