#include <stdlib.h>
#include <stdio.h>

#include "../type-info.h"
#include "../memory.h"

#include "primitives.h"

struct NPrimitive {
	NObject parent;
	NPrimitiveFunction function;
	void* data;
};

static NType   _type;
static int32_t _type_id;

void
n_init_primitives() {
	NError error;
	NTypeRegistry* registry = n_type_registry_get_default();

	n_type_init(&_type, "org.nuvm.PrimitiveProcedure");

	_type_id = n_type_registry_add_type(registry, &_type, &error);
	if (error.code != N_E_OK) {
		fprintf(stderr, "Unable to register type for Primitive Procedures. "
		                "Aborting.\n");
		exit(1);
	}
}


bool
n_is_primitive(NValue val) {
	return n_typeof(val) == _type_id;
}


NPrimitive*
n_primitive_new(NPrimitiveFunction function, void* data, NError* error) {
	NPrimitive* self;

	n_error_clear(error);

	if (function == NULL) {
		n_error_set(error, N_E_INVALID_ARGUMENT);
		n_error_set_msg(error, "function");
		return NULL;
	}

	self = n_alloc(sizeof(NPrimitive));
	if (self == NULL) {
		n_error_set(error, N_E_BAD_ALLOCATION);
		n_error_set_msg(error, "self");
	}

	self->parent.type_id = _type_id;
	self->function = function;
	self->data = data;

	return self;

}


void
n_primitive_destroy(NPrimitive* self) {
	n_free(self);
}


NValue
n_primitive_call(NPrimitive* self, NError* error) {
	n_error_clear(error);
	return self->function(self->data, error);
}
