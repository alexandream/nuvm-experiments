#include <stdlib.h>
#include <stdio.h>

#include "../type-info.h"
#include "../memory.h"
#include "../module.h"

#include "procedures.h"

struct NProcedure {
	NObject parent;
	NModule* module;
	uint8_t num_locals;
	uint32_t entry_point;
};

static NType   _type;
static int32_t _type_id;

void
n_init_procedures() {
	NError error;
	NTypeRegistry* registry = n_type_registry_get_default();

	n_type_init(&_type, "org.nuvm.Procedure");

	_type_id = n_type_registry_add_type(registry, &_type, &error);
	if (error.code != N_E_OK) {
		printf("Error: %u.\n", error.code);
		fprintf(stderr, "Unable to register type for Procedures. "
		                "Aborting.\n");
		exit(1);
	}
}


NProcedure*
n_procedure_new(NModule* mod,
                uint32_t entry,
                uint8_t nlocals,
                NError* error) {
	NProcedure* self;

	n_error_clear(error);

	if (mod == NULL) {
		n_error_set(error, N_E_INVALID_ARGUMENT);
		n_error_set_msg(error, "module");
		return NULL;
	}

	if (entry >= n_module_get_code_size(mod)) {
		n_error_set(error, N_E_INVALID_ARGUMENT);
		n_error_set_msg(error, "entry_point");
		return NULL;
	}

	self = n_alloc(sizeof(NProcedure));
	if (self == NULL) {
		n_error_set(error, N_E_BAD_ALLOCATION);
		n_error_set_msg(error, "self");
		return NULL;
	}
	self->parent.type_id = _type_id;
	self->module = mod;
	self->num_locals = nlocals;
	self->entry_point = entry;
	return self;
}


bool
n_is_procedure(NValue value) {
	return n_typeof(value) == _type_id;
}


uint8_t
n_procedure_count_locals(NProcedure* self) {
	return self->num_locals;
}


uint32_t
n_procedure_get_entry_point(NProcedure* self) {
	return self->entry_point;
}


NModule*
n_procedure_get_module(NProcedure* self) {
	return self->module;
}
