#include <stdlib.h>
#include <stdio.h>

#include "error.h"

#include "value.h"
#include "type-info.h"

static NType   _bool_type;
static int32_t _bool_type_id;

static NType   _fixnum_type;
static int32_t _fixnum_type_id;

static NType   _undefined_type;
static int32_t _undefined_type_id;


void
n_init_values() {
	NError error;
	NTypeRegistry* registry = n_type_registry_get_default();

	n_type_init(&_bool_type, "org.nuvm.Boolean");
	_bool_type_id = n_type_registry_add_type(registry, &_bool_type, &error);
	if (error.code != N_E_OK) {
		fprintf(stderr, "Unable to register type for Booleans. "
		                "Aborting.\n");
		exit(1);
	}


	n_type_init(&_fixnum_type, "org.nuvm.Fixnum32");
	_fixnum_type_id = n_type_registry_add_type(registry,
	                                           &_fixnum_type,
	                                           &error);
	if (error.code != N_E_OK) {
		fprintf(stderr, "Unable to register type for Fixnums. "
		                "Aborting.\n");
		exit(1);
	}

	n_type_init(&_undefined_type, "org.nuvm.Undefined");
	_undefined_type_id = n_type_registry_add_type(registry,
	                                              &_undefined_type,
	                                              &error);
	if (error.code != N_E_OK) {
		fprintf(stderr, "Unable to register type for Undefined. "
		                "Aborting.\n");
		exit(1);
	}
}


NValue
n_wrap_pointer(void* pointer) {
	NValue result;
	result.pointer = pointer;
	return result;
}


void*
n_unwrap_pointer(NValue value) {
	return value.pointer;
}

bool
n_is_equal(NValue value1, NValue value2) {
	return value1.contents == value2.contents;
}

bool
n_is_pointer(NValue value) {
	return true;
}

int32_t n_typeof(NValue value) {
	if (n_is_pointer(value)) {
		NObject* obj = n_unwrap_pointer(value);
		return obj->type_id;
	}
	return -1;
}
