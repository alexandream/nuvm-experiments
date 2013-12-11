#include <stdlib.h>
#include "memory.h"

#include "module.h"

#define ARRAY_TYPE_NAME     NRegisterArray
#define ARRAY_PREFIX        n_register_array
#define ARRAY_CONTENTS_TYPE NValue
#define ARRAY_ELEMENTS__SKIP
#include "util/array.h"

#define ARRAY_TYPE_NAME NInstructionArray
#define ARRAY_PREFIX    n_instruction_array
#define ARRAY_CONTENTS_TYPE NInstruction
#define ARRAY_SIZE_T int64_t
#define ARRAY_ELEMENTS__SKIP
#include "util/array.h"

struct NModule {
	NRegisterArray registers;
	NInstructionArray code;
	uint16_t entry_point;
};

NModule*
n_module_new(uint16_t register_count, uint16_t entry, uint32_t code_size,
             NError* error) {
	NModule* self = NULL;
	n_error_clear(error);
	if (register_count == 0) {
		n_error_set(error, N_E_INVALID_ARGUMENT);
		n_error_set_msg(error, "register_count");
		return NULL;
	}

	if (entry >= register_count) {
		n_error_set(error, N_E_INVALID_ARGUMENT);
		n_error_set_msg(error, "entry_point");
		return NULL;
	}

	self = (NModule*) n_alloc_unmanaged(sizeof(NModule));
	if (self == NULL) {
		n_error_set(error, N_E_BAD_ALLOCATION);
		n_error_set_msg(error, "self");
		return NULL;
	}

	n_register_array_init(&self->registers, register_count);
	n_instruction_array_init(&self->code, code_size);

	self->entry_point = entry;

	return self;
}


void
n_module_destroy(NModule* self) {
	if (self != NULL) {
		n_register_array_destroy(&self->registers);
		n_instruction_array_destroy(&self->code);
		n_free(self);
	}
}


NInstruction
n_module_fetch(NModule* self, uint32_t index, NError* error) {
	n_error_clear(error);

	if (index >= n_instruction_array_size(&self->code)) {
		NInstruction dummy;
		n_error_set(error, N_E_INVALID_ARGUMENT);
		n_error_set_msg(error, "index");
		return dummy;
	}

	return n_instruction_array_get(&self->code, index);
}


NValue
n_module_get_entry_value(NModule* self, NError* error) {
	return n_module_get_register(self, self->entry_point, error);
}


NValue
n_module_get_register(NModule* self, uint16_t index, NError* error) {
	NValue result;

	if (index >= n_register_array_size(&self->registers)) {
		n_error_set(error, N_E_INVALID_ARGUMENT);
		n_error_set_msg(error, "index");
		return result;
	}
	result = n_register_array_get(&self->registers, index);
	return result;
}


void
n_module_set_instruction(NModule* self,
                         uint32_t index,
                         NInstruction inst,
                         NError* error) {
	if (index >= n_instruction_array_size(&self->code)) {
		n_error_set(error, N_E_INVALID_ARGUMENT);
		n_error_set_msg(error, "index");
		return;
	}
	n_instruction_array_set(&self->code, index, inst);
}


void
n_module_set_register(NModule* self, uint16_t index,
                      NValue val, NError* error) {
	n_error_clear(error);

	if (index >= n_register_array_size(&self->registers)) {
		n_error_set(error, N_E_INVALID_ARGUMENT);
		n_error_set_msg(error, "index");
		return;
	}

	n_register_array_set(&self->registers, index, val);
}
