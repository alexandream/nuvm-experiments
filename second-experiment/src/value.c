#include <stdlib.h>
#include <stdio.h>

#include "error.h"

#include "value.h"
#include "type-info.h"
#include "memory.h"

#include "objects/strings.h"

#define HALF_TAG_POINTER 0x0000u
#define TAG_FIXNUM       0xFFFFFFFFu
#define TAG_SYMBOL       0xFFFFFFFEu

static NType   _bool_type;
static int32_t _bool_type_id;

static NType   _fixnum_type;
static int32_t _fixnum_type_id;

static NType   _symbol_type;
static int32_t _symbol_type_id;

static NType   _undefined_type;
static int32_t _undefined_type_id;

NValue N_TRUE;
NValue N_FALSE;
NValue N_UNDEFINED;

void
n_init_values() {
	NError error;
	NObject *obj_true, *obj_false, *obj_undefined;
	NTypeRegistry* registry = n_type_registry_get_default();

	n_type_init(&_bool_type, "org.nuvm.Boolean");
	_bool_type_id = n_type_registry_add_type(registry, &_bool_type, &error);
	if (error.code != N_E_OK) {
		fprintf(stderr, "Unable to register type for Booleans. "
		                "Aborting.\n");
		exit(1);
	}

	n_type_init(&_fixnum_type, "org.nuvm.Fixnum32");
	_fixnum_type_id =
		n_type_registry_add_type(registry, &_fixnum_type, &error);
	if (error.code != N_E_OK) {
		fprintf(stderr, "Unable to register type for Fixnums. "
		                "Aborting.\n");
		exit(1);
	}

	n_type_init(&_undefined_type, "org.nuvm.Undefined");
	_undefined_type_id =
		n_type_registry_add_type(registry, &_undefined_type, &error);

	if (error.code != N_E_OK) {
		fprintf(stderr, "Unable to register type for Undefined. "
		                "Aborting.\n");
		exit(1);
	}

	n_type_init(&_symbol_type, "org.nuvm.Symbol");
	_symbol_type_id =
		n_type_registry_add_type(registry, &_symbol_type, &error);

	if (error.code != N_E_OK) {
		fprintf(stderr, "Unable to register type for Symbols. Aborting.\n");
		exit(1);
	}

	obj_true = n_alloc_unmanaged(sizeof(NObject));
	obj_true->type_id = _bool_type_id;
	N_TRUE = n_wrap_pointer(obj_true);

	obj_false = n_alloc_unmanaged(sizeof(NObject));
	obj_false->type_id = _bool_type_id;
	N_FALSE = n_wrap_pointer(obj_false);

	obj_undefined = n_alloc_unmanaged(sizeof(NObject));
	obj_undefined->type_id = _undefined_type_id;
	N_UNDEFINED = n_wrap_pointer(obj_undefined);
}


NValue
n_wrap_fixnum(int32_t fixnum) {
	NValue result;
	result.contents = 0L;
	result.immediate.data.fixnum = fixnum;
	result.immediate.tags.full_tag = TAG_FIXNUM;
	return result;
}


NValue
n_wrap_pointer(void* pointer) {
	NValue result;
	result.contents = 0L;
	result.pointer = pointer;
	return result;
}


NValue
n_wrap_symbol(int32_t symbol_id) {
	NValue result;
	result.contents = 0L;
	result.immediate.tags.full_tag = TAG_SYMBOL;
	result.immediate.data.symbol_id = symbol_id;
	return result;
}


int32_t
n_unwrap_fixnum(NValue value) {
	return value.immediate.data.fixnum;
}


void*
n_unwrap_pointer(NValue value) {
	return value.pointer;
}


int32_t
n_unwrap_symbol(NValue value) {
	return value.immediate.data.symbol_id;
}


bool
n_is_boolean(NValue value) {
	return n_typeof(value) == _bool_type_id;
}


bool
n_is_equal(NValue value1, NValue value2) {
	return value1.contents == value2.contents;
}


bool
n_is_fixnum(NValue value) {
	return value.immediate.tags.full_tag == TAG_FIXNUM;
}


bool
n_is_pointer(NValue value) {
	return value.immediate.tags.half_tags.upper_tag == HALF_TAG_POINTER;
}


bool
n_is_symbol(NValue value) {
	return value.immediate.tags.full_tag == TAG_SYMBOL;
}


bool
n_is_undefined(NValue value) {
	return n_typeof(value) == _undefined_type_id;
}


NValue
n_to_string(NValue value) {
	NTypeRegistry* registry = n_type_registry_get_default();
	NType* type = n_type_registry_fetch_type(registry, n_typeof(value));

	/* FIXME: Should manage the error result on string constructor and what
	   to do when type->to_string returns NULL */
	char* repr_bytes = type->to_string(type, value);
	NString* repr_string = n_string_wrap(repr_bytes, NULL);

	return n_wrap_pointer(repr_string);
}



int32_t n_typeof(NValue value) {
	if (n_is_pointer(value)) {
		NObject* obj = n_unwrap_pointer(value);
		return obj->type_id;
	}
	return -1;
}
