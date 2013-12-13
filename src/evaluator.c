#include <stdlib.h>
#include <stdio.h>

#include "memory.h"

#include "evaluator.h"
#include "instruction.h"
#include "objects/primitives.h"
#include "objects/procedures.h"

/*
static NValue
_get_global(NEvaluator*, uint16_t);

static NValue
_get_local(NEvaluator*, uint8_t);
*/
static uint32_t
_op_global_ref(NEvaluator*, NInstruction);

static uint32_t
_op_global_set(NEvaluator*, NInstruction);
/*
static uint32_t
_op_return(NEvaluator*, NInstruction);
*/
static NValue
_run_loop(NEvaluator*, NProcedure*, NError*);
/*
static void
_set_global(NEvaluator*, uint16_t, NValue);

static void
_set_local(NEvaluator*, uint8_t, NValue);
*/
static bool
_step(NEvaluator*, NValue*, NError*);


struct NEvaluator {
	NModule* current_module;
	uint32_t code_pointer;

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
	self->code_pointer = 0;
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

	if (n_is_procedure(entry)) {
		return _run_loop(self, n_unwrap_pointer(entry), error);
	}
	else if (n_is_primitive(entry)) {
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


/* ----- Auxiliary functions. ----- */

static uint32_t
_op_global_ref(NEvaluator* self, NInstruction inst) {
	return self->code_pointer +1;
}


static uint32_t
_op_global_set(NEvaluator* self, NInstruction inst) {
	return self->code_pointer +1;
}


static NValue
_run_loop(NEvaluator* self, NProcedure* proc, NError* error) {
	NValue result;
	bool halt = false;
	while (!halt) {
		halt = _step(self, &result, error);
	}
	return result;
}


static bool
_step(NEvaluator* self, NValue* result, NError* error) {
	uint32_t next_instruction;
	bool halt = false;
	NInstruction inst =
		n_module_fetch(self->current_module,
		               self->code_pointer,
		               NULL); /* TODO: Handle errors here. */

	switch(inst.base.opcode) {
		case N_OP_GLOBAL_REF:
			next_instruction = _op_global_ref(self, inst);
			break;
		case N_OP_GLOBAL_SET:
			next_instruction = _op_global_set(self, inst);
			break;
		case N_OP_RETURN:
			halt = true;
			break;
		default:
			n_error_set(error, N_E_UNKNOWN_OPCODE);
			n_error_set_msgf(error, "0x%02X", inst.base.opcode);
			break;
	}
	return halt;
}
