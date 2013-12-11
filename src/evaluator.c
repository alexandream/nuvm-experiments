#include <stdlib.h>
#include <stdio.h>

#include "memory.h"

#include "evaluator.h"
#include "objects/primitives.h"

struct NEvaluator {
	NModule* current_module;
};


NEvaluator*
n_evaluator_new(NError* error) {
	NEvaluator* self = (NEvaluator*) n_alloc_unmanaged(sizeof(NEvaluator));
	if (self == NULL) {
		n_error_set(error, N_E_BAD_ALLOCATION);
		n_error_set_msg(error, "self");
		return NULL;
	}

	self->current_module = NULL;

	return self;
}


void
n_evaluator_destroy(NEvaluator* self) {
	n_free(self);
}


NValue
n_evaluator_run(NEvaluator* self, NError* error) {
	NValue entry;
	NValue result;
	n_error_clear(error);
	if (self->current_module == NULL) {
		n_error_set(error, N_E_UNINITIALIZED_EVALUATOR);
		return result;
	}

	entry = n_module_get_entry_value(self->current_module, error);
	if (!n_error_ok(error)) {
		return result;
	}

	if (n_is_primitive(entry)) {
		return n_primitive_call(n_unwrap_pointer(entry), error);
	}
	else {
		return entry;
	}
}
void
n_evaluator_setup(NEvaluator* self, NModule* mod) {
	self->current_module = mod;
}
