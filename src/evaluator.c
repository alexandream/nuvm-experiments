#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

#include "memory.h"

#include "evaluator.h"
#include "instruction.h"
#include "objects/primitives.h"
#include "objects/procedures.h"


#define ARRAY_TYPE_NAME     NStack
#define ARRAY_PREFIX        n_stack
#define ARRAY_CONTENTS_TYPE NValue
#define ARRAY_ELEMENTS__SKIP
#include "util/array.h"

#define N_STACK_SIZE 8192

#define N_STACK_CALLER_SLOT        0
#define N_STACK_CODE_POINTER_SLOT  1
#define N_STACK_STACK_POINTER_SLOT 2
#define N_STACK_RETURN_SLOT        3
#define N_STACK_NUM_LOCALS_SLOT    4

#define N_STACK_SLOTS 5


static NValue
_get_frame_caller(NEvaluator* self);

static uint8_t
_get_frame_num_locals(NEvaluator* self);

static NValue
_get_global(NEvaluator*, uint16_t);

static NValue
_get_local(NEvaluator*, uint8_t);

static uint32_t
_op_call_sva(NEvaluator*, NInstruction);

static uint32_t
_op_global_ref(NEvaluator*, NInstruction);

static uint32_t
_op_global_set(NEvaluator*, NInstruction);

static uint32_t
_op_jump(NEvaluator* self, NInstruction inst);

static uint32_t
_op_jump_if(NEvaluator* self, NInstruction inst);

static uint32_t
_op_jump_unless(NEvaluator* self, NInstruction inst);

static uint32_t
_op_return(NEvaluator*, NInstruction, bool* halt, NValue* result);

static NValue
_run_procedure(NEvaluator*, NProcedure*, NError*);

static void
_set_frame_caller(NEvaluator* self, NValue caller);

static void
_set_frame_code_pointer(NEvaluator* self, uint32_t code_pointer);

static void
_set_frame_num_locals(NEvaluator* self, uint8_t num_locals);

static void
_set_frame_return_storage(NEvaluator* self, uint8_t return_storage);

static void
_set_frame_stack_pointer(NEvaluator* self, int32_t stack_pointer);

static void
_set_global(NEvaluator*, uint16_t, NValue);

static void
_set_local(NEvaluator*, uint8_t, NValue);

static uint32_t
_step(NEvaluator*, NValue*, bool*, NError*);


struct NEvaluator {
	NModule* current_module;
	uint32_t code_pointer;
	int32_t stack_pointer;
	NStack stack;
	NValue locals[256];
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
	self->stack_pointer = 0;
	n_stack_init(&self->stack, N_STACK_SIZE);
	return self;
}


void
n_evaluator_destroy(NEvaluator* self) {
	n_stack_destroy(&self->stack);
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
		return _run_procedure(self, n_unwrap_pointer(entry), error);
	}
	else if (n_is_primitive(entry)) {
		return n_primitive_call(n_unwrap_pointer(entry), N_UNDEFINED, error);
	}
	else {
		return entry;
	}
}


void
n_evaluator_setup(NEvaluator* self, NModule* mod) {
	self->current_module = mod;
}


int32_t
n_evaluator_stack_capacity(NEvaluator* self) {
	return n_stack_size(&self->stack);
}

int32_t
n_evaluator_stack_pointer(NEvaluator* self) {
	return self->stack_pointer;
}

/* ----- Auxiliary functions. ----- */

static int32_t
_calculate_next_stack_pointer(NEvaluator* self) {
	uint8_t n_locals = _get_frame_num_locals(self);
	return self->stack_pointer + n_locals + N_STACK_SLOTS;
}


static NValue
_get_frame_caller(NEvaluator* self) {
	int32_t caller_index = self->stack_pointer + N_STACK_CALLER_SLOT;
	return n_stack_get(&self->stack, caller_index);
}


static uint8_t
_get_frame_num_locals(NEvaluator* self) {
	int32_t num_locals_index = self->stack_pointer + N_STACK_NUM_LOCALS_SLOT;
	return (uint8_t) n_stack_get(&self->stack, num_locals_index).contents;
}


static NValue
_get_global(NEvaluator* self, uint16_t index) {
	return n_module_get_register(self->current_module, index, NULL);
}


static NValue
_get_local(NEvaluator* self, uint8_t index) {
	return self->locals[index];
}


static void
_push_stack_frame(NEvaluator* self,
                  NValue caller,
                  uint8_t return_storage,
                  uint8_t num_locals) {
	int32_t old_stack_pointer = self->stack_pointer;

	self->stack_pointer = _calculate_next_stack_pointer(self);
	
	_set_frame_caller(self, caller);
	_set_frame_code_pointer(self, self->code_pointer);
	_set_frame_stack_pointer(self, old_stack_pointer);
	_set_frame_return_storage(self, return_storage);
	_set_frame_num_locals(self, num_locals);
}



static uint32_t
_op_call_sva(NEvaluator* self, NInstruction inst) {
	NValue func_val, arg, result;
	uint8_t l_dest, l_func, l_arg;
	NPrimitive* prim;
	
	n_decode_call_sva(inst, &l_dest, &l_func, &l_arg);
	func_val = _get_local(self, l_func);
	arg = _get_local(self, l_arg);
	
	assert(n_is_primitive(func_val));

	prim = n_unwrap_pointer(func_val);
	result = n_primitive_call(prim, arg, NULL);
	_set_local(self, l_dest, result);

	return self->code_pointer +1;
}


static uint32_t
_op_global_ref(NEvaluator* self, NInstruction inst) {
	uint8_t dest;
	uint16_t source;

	n_decode_global_ref(inst, &dest, &source);

	_set_local(self, dest, _get_global(self, source));

	return self->code_pointer +1;
}


static uint32_t
_op_global_set(NEvaluator* self, NInstruction inst) {
	uint16_t dest;
	uint8_t source;

	n_decode_global_set(inst, &dest, &source);
	_set_global(self, dest, _get_local(self, source));

	return self->code_pointer +1;
}


static uint32_t
_op_jump(NEvaluator* self, NInstruction inst) {
	int32_t offset;
	
	n_decode_jump(inst, &offset);
	return self->code_pointer + offset;
}


static uint32_t
_op_jump_if(NEvaluator* self, NInstruction inst) {
	uint8_t cond;
	int16_t offset;
	
	uint32_t result = self->code_pointer +1;

	n_decode_jump_if(inst, &cond, &offset);
	if (!n_is_equal(_get_local(self, cond), N_FALSE)) {
		result = self->code_pointer + offset;
	}
	return result;
}


static uint32_t
_op_jump_unless(NEvaluator* self, NInstruction inst) {
	uint8_t cond;
	int16_t offset;
	
	uint32_t result = self->code_pointer +1;
	
	n_decode_jump_if(inst, &cond, &offset);
	if (n_is_equal(_get_local(self, cond), N_FALSE)) {
		result = self->code_pointer + offset;
	}
	return result;

}


static uint32_t
_op_return(NEvaluator* self, NInstruction inst, bool* halt, NValue* result) {
	uint8_t source;
	NValue caller = _get_frame_caller(self);

	n_decode_return(inst, &source);
	if (n_is_equal(caller, N_UNDEFINED)) {
		*result = _get_local(self, source);
		*halt = true;
	}
	return self->code_pointer;
}


static NValue
_run_procedure(NEvaluator* self, NProcedure* proc, NError* error) {
	NError inner_error;
	NValue result;
	uint8_t num_locals = n_procedure_count_locals(proc);
	bool halt = false;
	
	self->current_module = n_procedure_get_module(proc);
	self->code_pointer = n_procedure_get_entry_point(proc);
	_push_stack_frame(self, N_UNDEFINED, num_locals, 0);

	if (error == NULL) {
		error = &inner_error;
	}
	
	while (!halt) {
		self->code_pointer = _step(self, &result, &halt, error);
		if (error->code != N_E_OK) {
			halt = true;
		}	
	}
	return result;
}


static void
_set_frame_caller(NEvaluator* self, NValue caller) {
	int32_t slot = self->stack_pointer + N_STACK_CALLER_SLOT;
	n_stack_set(&self->stack, slot, caller);
}


static void
_set_frame_code_pointer(NEvaluator* self, uint32_t code_pointer) {
	int32_t slot = self->stack_pointer + N_STACK_CODE_POINTER_SLOT;
	NValue cp_val;
	cp_val.contents = (uint64_t) code_pointer;
	n_stack_set(&self->stack, slot, cp_val);
}


static void
_set_frame_num_locals(NEvaluator* self, uint8_t num_locals) {
	int32_t slot = self->stack_pointer + N_STACK_NUM_LOCALS_SLOT;
	NValue num_locals_val;
	num_locals_val.contents = (uint64_t) num_locals;
	n_stack_set(&self->stack, slot, num_locals_val);
}


static void
_set_frame_return_storage(NEvaluator* self, uint8_t return_storage) {
	int32_t slot = self->stack_pointer + N_STACK_RETURN_SLOT;
	NValue return_storage_val;
	return_storage_val.contents = (uint64_t) return_storage;
	n_stack_set(&self->stack, slot, return_storage_val);
}


static void
_set_frame_stack_pointer(NEvaluator* self, int32_t stack_pointer) {
	int32_t slot = self->stack_pointer + N_STACK_STACK_POINTER_SLOT;
	NValue sp_val;
	sp_val.contents = (uint64_t) stack_pointer;
	n_stack_set(&self->stack, slot, sp_val);
}


static void
_set_global(NEvaluator* self, uint16_t index, NValue val) {
	n_module_set_register(self->current_module, index, val, NULL);
}


static void
_set_local(NEvaluator* self, uint8_t index, NValue val) {
	self->locals[index] = val;
}


static uint32_t
_step(NEvaluator* self, NValue* result, bool* halt, NError* error) {
	uint32_t next_instruction;
	NInstruction inst =
		n_module_fetch(self->current_module,
		               self->code_pointer,
		               NULL); /* TODO: Handle errors here. */

	switch(inst.base.opcode) {
		case N_OP_CALL_SVA:
			next_instruction = _op_call_sva(self, inst);
			break;
		case N_OP_GLOBAL_REF:
			next_instruction = _op_global_ref(self, inst);
			break;
		case N_OP_GLOBAL_SET:
			next_instruction = _op_global_set(self, inst);
			break;
		case N_OP_JUMP:
			next_instruction = _op_jump(self, inst);
			break;
		case N_OP_JUMP_IF:
			next_instruction = _op_jump_if(self, inst);
			break;
		case N_OP_JUMP_UNLESS:
			next_instruction = _op_jump_unless(self, inst);
			break;
		case N_OP_RETURN:
			next_instruction = _op_return(self, inst, halt, result);
			break;
		default:
			n_error_set(error, N_E_UNKNOWN_OPCODE);
			n_error_set_msgf(error, "0x%02X", inst.base.opcode);
			next_instruction = self->code_pointer;
			break;
	}
	return next_instruction;
}
