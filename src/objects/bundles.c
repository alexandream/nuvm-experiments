#include <stdlib.h>
#include <stdio.h>

#include "../error.h"
#include "../memory.h"
#include "../type-info.h"
#include "../value.h"

#include "bundles.h"

#define ARRAY_TYPE_NAME     NValueArray
#define ARRAY_PREFIX        n_value_array
#define ARRAY_CONTENTS_TYPE NValue
#define ARRAY_DESTROY__SKIP
#define ARRAY_GET__SKIP
#define ARRAY_SET__SKIP
#define ARRAY_ELEMENTS__SKIP
#include "../util/array.h"

struct NBundle {
	NObject parent;
	NValueArray slots;
	/* FIXME: Using an array, but a mapping structure would be way more
	 * appropriate. */
	NValueArray mapper;
};

static NType   _type;
static int32_t _type_id;

void
n_init_bundles() {
	NError error;
	NTypeRegistry* registry = n_type_registry_get_default();

	n_type_init(&_type, "org.nuvm.Bundle");

	_type_id = n_type_registry_add_type(registry, &_type, &error);
	if (error.code != N_E_OK) {
		fprintf(stderr, "Unable to register type for Bundles. Aborting.\n");
		exit(1);
	}
}


NBundle*
n_bundle_new(uint16_t size, NError* error) {
	NBundle* self = NULL;
	n_error_clear(error);

	if (size == 0) {
		n_error_set(error, N_E_INVALID_ARGUMENT);
		n_error_set_msg(error, "size");
		return NULL;
	}

	self = n_alloc(sizeof(NBundle));
	if (self == NULL) {
		n_error_set(error, N_E_BAD_ALLOCATION);
		n_error_set_msg(error, "self");
	}
	/* FIXME: On allocation failure, both arrays stay unchecked. */
	n_value_array_init(&self->slots, size);
	n_value_array_init(&self->mapper, size);
	return self;
}


uint16_t
n_bundle_size(NBundle* self) {
	return n_value_array_size(&self->slots);
}
