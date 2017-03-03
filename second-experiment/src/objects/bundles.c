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
#include "../util/array.h"

struct NBundle {
	NObject parent;
	/* FIXME: Using two arrays for simplicity, but eventually we'll migrate
	 * this setup to a single sorted array easier to make binary searches.
	 * The binary searches might still be too slow, so the open/close states
	 * give us an opportunity to optimize slots representation during closing
	 * if the need arises. */
	int32_t next_slot;
	bool closed;
	NValueArray slots;
	NValueArray mapper;
};

static NType   _type;
static int32_t _type_id;


static int32_t
_find_slot(NBundle* self, NValue symbol, bool insert);


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
		return NULL;
	}
	self->parent.type_id = _type_id;
	self->closed = false;
	self->next_slot = 0;

	n_value_array_init(&self->slots, size);
	if (n_value_array_elements(&self->slots) == NULL) {
		n_error_set(error, N_E_BAD_ALLOCATION);
		n_error_set_msg(error, "slots");
		return NULL;
	}

	n_value_array_init(&self->mapper, size);
	if (n_value_array_elements(&self->mapper) == NULL) {
		n_value_array_destroy(&self->slots);
		n_error_set(error, N_E_BAD_ALLOCATION);
		n_error_set_msg(error, "mapper");
		return NULL;
	}

	return self;
}


void
n_bundle_close(NBundle* self) {
	self->closed = true;
}


NValue
n_bundle_get(NBundle* self, NValue symbol, NError* error) {
	int32_t slot;
	n_error_clear(error);

	if (!self->closed) {
		n_error_set(error, N_E_INVALID_STATE);
		n_error_set_msg(error, "open");
		return N_UNDEFINED;
	}
	slot = _find_slot(self, symbol, false);
	if (slot < 0) {
		return N_UNDEFINED;
	}
	else {
		return n_value_array_get(&self->slots, slot);
	}
}


void
n_bundle_set(NBundle* self, NValue symbol, NValue value, NError* error) {
	int32_t slot;
	n_error_clear(error);

	if (self->closed) {
		n_error_set(error, N_E_INVALID_STATE);
		n_error_set_msg(error, "closed");
		return;
	}
	slot = _find_slot(self, symbol, true);
	if (slot < 0) {
		n_error_set(error, N_E_INVALID_STATE);
		n_error_set_msg(error, "overflow");
		return;
	}
	n_value_array_set(&self->slots, slot, value);
	n_value_array_set(&self->mapper, slot, symbol);
}


uint16_t
n_bundle_size(NBundle* self) {
	return n_value_array_size(&self->slots);
}


bool
n_is_bundle(NValue value) {
	return n_typeof(value) == _type_id;
}


/* ----- Auxiliary Functions ----- */

static int32_t
_find_slot(NBundle* self, NValue symbol, bool insert) {
	int32_t i, index;
	int32_t nslots = n_value_array_size(&self->slots);
	for (i = 0; i < nslots; i++) {
		if (n_is_equal(symbol, n_value_array_get(&self->mapper, i))) {
			return i;
		}
	}
	if (insert) {
		index = self->next_slot;
		if (index >= nslots) {
			index = -1;
		}
		else if (index >= 0) {
			self->next_slot++;
		}
		return index;
	}
	else {
		return -1;
	}
}
