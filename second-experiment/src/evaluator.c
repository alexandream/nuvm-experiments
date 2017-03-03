#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

#include "memory.h"
#include "loader.h"

#include "evaluator.h"
#include "instruction.h"
#include "objects/bundles.h"
#include "objects/primitives.h"
#include "objects/procedures.h"
#include "objects/strings.h"


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
#define N_STACK_NUM_ARGS_SLOT      5

#define N_STACK_SLOTS 6


static NValue
_get_frame_caller(NEvaluator* self);

static uint32_t
_get_frame_code_pointer(NEvaluator* self);

static uint8_t
_get_frame_num_locals(NEvaluator* self);

static int32_t
_get_frame_stack_pointer(NEvaluator* self);

static NValue
_get_global(NEvaluator*, uint16_t);

static NValue
_get_local(NEvaluator*, uint8_t);

static NValue
_run_procedure(NEvaluator*, NProcedure*, NError*);

static void
_set_argument(NEvaluator* self, uint8_t index, NValue value);

static void
_set_frame_caller(NEvaluator* self, NValue caller);

static void
_set_frame_code_pointer(NEvaluator* self, uint32_t code_pointer);

static void
_set_frame_num_args(NEvaluator* self, uint8_t num_args);

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
	NProcedure* current_procedure;
	uint32_t code_pointer;
	int32_t stack_pointer;
	NStack stack;
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
	self->current_procedure = NULL;
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
	n_error_clear(error);
	if (self->current_module == NULL) {
		n_error_set(error, N_E_UNINITIALIZED_EVALUATOR);
		return N_UNDEFINED;
	}

	entry = n_module_get_entry_value(self->current_module, error);
	if (!n_error_ok(error)) {
		return N_UNDEFINED;
	}

	if (n_is_procedure(entry)) {
		return _run_procedure(self, n_unwrap_pointer(entry), error);
	}
	else if (n_is_primitive(entry)) {
		return n_primitive_call(n_unwrap_pointer(entry), NULL, 0, error);
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
	return self->stack_pointer + n_locals + N_STACK_SLOTS + 1;
}


static uint32_t
_call_procedure(NEvaluator* self,
                NProcedure* callee,
                uint8_t l_dest,
                NValue* args,
                uint8_t nargs,
				uint8_t skip_blocks) {
	NProcedure* caller = self->current_procedure;
	int32_t old_stack_pointer = self->stack_pointer;
	uint8_t num_locals = n_procedure_count_locals(callee);

	NValue caller_val = (caller != NULL) ? n_wrap_pointer(caller)
	                                     : N_UNDEFINED;

	self->stack_pointer = _calculate_next_stack_pointer(self);

	_set_frame_caller(self, caller_val);
	_set_frame_code_pointer(self, self->code_pointer + skip_blocks +1);
	_set_frame_stack_pointer(self, old_stack_pointer);
	_set_frame_return_storage(self, l_dest);
	_set_frame_num_locals(self, num_locals);
	_set_frame_num_args(self, nargs);

	{
		int i;
		for (i = 0; i < nargs; i++) {
			_set_argument(self, i, args[i]);
		}
	}

	self->current_module = n_procedure_get_module(callee);
	self->current_procedure = callee;
	return n_procedure_get_entry_point(callee);
}


static NValue
_get_frame_caller(NEvaluator* self) {
	int32_t caller_index = self->stack_pointer + N_STACK_CALLER_SLOT;
	return n_stack_get(&self->stack, caller_index);
}


static uint32_t
_get_frame_code_pointer(NEvaluator* self) {
	int32_t index = self->stack_pointer + N_STACK_CODE_POINTER_SLOT;
	return (uint32_t) n_stack_get(&self->stack, index).contents;
}


static uint8_t
_get_frame_num_locals(NEvaluator* self) {
	int32_t num_locals_index = self->stack_pointer + N_STACK_NUM_LOCALS_SLOT;
	return (uint8_t) n_stack_get(&self->stack, num_locals_index).contents;
}


static uint8_t
_get_frame_return_storage(NEvaluator* self) {
	int32_t index = self->stack_pointer + N_STACK_RETURN_SLOT;
	return (uint8_t) n_stack_get(&self->stack, index).contents;
}


static int32_t
_get_frame_stack_pointer(NEvaluator* self) {
	int32_t stack_pointer_index =
		self->stack_pointer + N_STACK_STACK_POINTER_SLOT;
	return (int32_t) n_stack_get(&self->stack, stack_pointer_index).contents;
}


static NValue
_get_global(NEvaluator* self, uint16_t index) {
	return n_module_get_register(self->current_module, index, NULL);
}


static NValue
_get_local(NEvaluator* self, uint8_t index) {
	int32_t stack_index = self->stack_pointer + N_STACK_SLOTS + index;
	return n_stack_get(&self->stack, stack_index);
}


static void
_pop_stack_frame(NEvaluator* self) {
	NProcedure* caller = n_unwrap_pointer(_get_frame_caller(self));
	uint32_t code_pointer = _get_frame_code_pointer(self);

	self->stack_pointer = _get_frame_stack_pointer(self);
	self->code_pointer = code_pointer;
	self->current_procedure = caller;
	self->current_module = n_procedure_get_module(caller);
}


static uint32_t
_op_bundle_close(NEvaluator* self, NInstruction inst) {
	uint8_t local;
	NBundle* bundle;
	NValue value;

	n_decode_bundle_close(inst, &local);
	value = _get_local(self, local);
	assert(n_is_bundle(value));

	bundle = n_unwrap_pointer(value);
	n_bundle_close(bundle);

	return self->code_pointer +1;
}


static uint32_t
_op_bundle_get(NEvaluator* self, NInstruction inst) {
	uint8_t l_dest, l_bundle, l_symbol;
	NValue v_bundle, v_symbol, output;
	NBundle* bundle;

	n_decode_bundle_get(inst, &l_dest, &l_bundle, &l_symbol);
	v_bundle = _get_local(self, l_bundle);
	v_symbol = _get_local(self, l_symbol);

	assert(n_is_bundle(v_bundle));
	assert(n_is_symbol(v_symbol));

	bundle = n_unwrap_pointer(v_bundle);
	output = n_bundle_get(bundle, v_symbol, NULL);
	_set_local(self, l_dest, output);

	return self->code_pointer + 1;
}


static uint32_t
_op_bundle_set(NEvaluator* self, NInstruction inst) {
	uint8_t l_bundle, l_symbol, l_value;
	NValue v_bundle, v_symbol, value;
	NBundle* bundle;

	n_decode_bundle_set(inst, &l_bundle, &l_symbol, &l_value);
	value    = _get_local(self, l_value);
	v_bundle = _get_local(self, l_bundle);
	v_symbol = _get_local(self, l_symbol);

	assert(n_is_bundle(v_bundle));
	assert(n_is_symbol(v_symbol));

	bundle = n_unwrap_pointer(v_bundle);
	n_bundle_set(bundle, v_symbol, value, NULL);

	return self->code_pointer + 1;
}


static uint32_t
_op_call(NEvaluator* self, NInstruction inst) {
	NValue args[256];

	uint8_t l_dest, l_callee, nargs;
	uint32_t cp = self->code_pointer + 1;

	NValue callee;

	uint32_t num_blocks;
	uint8_t extra_args;
	uint8_t skip_blocks;
	bool has_tail_block;

	int i, bp;
	int32_t result;

	n_decode_call(inst, &l_dest, &l_callee, &nargs);

	num_blocks = nargs/4;
	extra_args = nargs % 4;
	has_tail_block = (extra_args > 0);
	skip_blocks = num_blocks + (has_tail_block ? 1 : 0);

	for (i = 0; i < num_blocks; i++) {
		NInstruction arg_pack =
			n_module_fetch(self->current_module, cp + i, NULL);

		bp = i * 4;
		args[bp + 0] = _get_local(self, arg_pack.extra.arg1);
		args[bp + 1] = _get_local(self, arg_pack.extra.arg2);
		args[bp + 2] = _get_local(self, arg_pack.extra.arg3);
		args[bp + 3] = _get_local(self, arg_pack.extra.arg4);
	}

	bp = num_blocks * 4;
	if (has_tail_block) {
		NInstruction arg_pack =
			n_module_fetch(self->current_module, cp + num_blocks, NULL);

		args[bp + 0] = _get_local(self, arg_pack.extra.arg1);
		if (extra_args > 1) {
			args[bp + 1] = _get_local(self, arg_pack.extra.arg2);
		}
		if (extra_args > 2) {
			args[bp + 2] = _get_local(self, arg_pack.extra.arg3);
		}
	}


	callee = _get_local(self, l_callee);
	if (n_is_primitive(callee)) {
		NPrimitive* primitive = n_unwrap_pointer(callee);
		NValue output = n_primitive_call(primitive, args, nargs, NULL);
		_set_local(self, l_dest, output);

		result = cp + skip_blocks;
	}
	else if (n_is_procedure(callee)) {
		NProcedure* proc = n_unwrap_pointer(callee);
		result = _call_procedure(self, proc, l_dest, args, nargs, skip_blocks);
	}
	else {
		assert(false);
	}


	return result;

}


static uint32_t
_op_call_sva(NEvaluator* self, NInstruction inst) {
	NValue callee, arg;
	uint8_t l_dest, l_func, l_arg;

	uint32_t result = 0;

	n_decode_call_sva(inst, &l_dest, &l_func, &l_arg);

	callee = _get_local(self, l_func);
	arg = _get_local(self, l_arg);

	if (n_is_primitive(callee)) {
		NPrimitive* prim = n_unwrap_pointer(callee);
		NValue output = n_primitive_call(prim, &arg, 1, NULL);

		_set_local(self, l_dest, output);
		result = self->code_pointer + 1;
	}
	else if (n_is_procedure(callee)) {
		NProcedure* proc = n_unwrap_pointer(callee);
		result = _call_procedure(self, proc, l_dest, &arg, 1, 0);
	}
	else {
		assert(false);
	}

	return result;
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
_op_load(NEvaluator* self, NInstruction inst) {
	uint8_t dest_local;
	uint8_t string_local;
	NValue string_val;
	const char* string;
	NValue loaded_value;

	uint32_t result = self->code_pointer +1;

	n_decode_load(inst, &dest_local, &string_local);

	string_val = _get_local(self, string_local);

	if (!n_is_string(string_val)) {
		/* FIXME: Should be throwing an error here, but opcodes can't
		 * report errors yet. */
	}
	string = n_string_contents((NString*)n_unwrap_pointer(string_val));
	/* FIXME: Again, this needs error reporting on opcodes. */
	loaded_value = n_load(string, NULL);
	_set_local(self, dest_local, loaded_value);

	return result;
}


static uint32_t
_op_new_bundle(NEvaluator* self, NInstruction inst) {
	uint8_t dest;
	uint16_t size;
	NBundle* bundle;

	n_decode_new_bundle(inst, &dest, &size);
	bundle = n_bundle_new(size, NULL);
	_set_local(self, dest, n_wrap_pointer(bundle));
	return self->code_pointer + 1;
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
	else {
		NValue output = _get_local(self, source);
		uint8_t l_dest = _get_frame_return_storage(self);
		_pop_stack_frame(self);
		_set_local(self, l_dest, output);
	}
	return self->code_pointer;
}


static NValue
_run_procedure(NEvaluator* self, NProcedure* proc, NError* error) {
	NError inner_error;
	NValue result = N_UNDEFINED;
	bool halt = false;
	self->current_module = n_procedure_get_module(proc);
	self->current_procedure = proc;
	self->code_pointer = n_procedure_get_entry_point(proc);

	_set_frame_caller(self, N_UNDEFINED);
	_set_frame_num_locals(self, n_procedure_count_locals(proc));
	_set_frame_num_args(self, 0);

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
_set_argument(NEvaluator* self, uint8_t index, NValue value) {
	int32_t nlocals = _get_frame_num_locals(self);
	int32_t stack_index =
	   self->stack_pointer + N_STACK_SLOTS + nlocals + index;
	n_stack_set(&self->stack, stack_index, value);
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
_set_frame_num_args(NEvaluator* self, uint8_t num_args) {
	int32_t slot = self->stack_pointer + N_STACK_NUM_ARGS_SLOT;
	NValue num_args_val;
	num_args_val.contents = (uint64_t) num_args;
	n_stack_set(&self->stack, slot, num_args_val);
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
	int32_t stack_index = self->stack_pointer + N_STACK_SLOTS + index;
	n_stack_set(&self->stack, stack_index, val);
}


static uint32_t
_step(NEvaluator* self, NValue* result, bool* halt, NError* error) {
	uint32_t next_instruction;
	NInstruction inst =
		n_module_fetch(self->current_module,
		               self->code_pointer,
		               NULL); /* TODO: Handle errors here. */
	switch(inst.base.opcode) {
		case N_OP_BUNDLE_CLOSE:
			next_instruction = _op_bundle_close(self, inst);
			break;
		case N_OP_BUNDLE_GET:
			next_instruction = _op_bundle_get(self, inst);
			break;
		case N_OP_BUNDLE_SET:
			next_instruction = _op_bundle_set(self, inst);
			break;
		case N_OP_CALL:
			next_instruction = _op_call(self, inst);
			break;
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
		case N_OP_LOAD:
			next_instruction = _op_load(self, inst);
			break;
		case N_OP_NEW_BUNDLE:
			next_instruction = _op_new_bundle(self, inst);
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
