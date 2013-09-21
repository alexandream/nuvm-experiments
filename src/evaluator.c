#include "util/common.h"
#include "util/converter.h"

#include "types/primitive.h"
#include "types/procedure.h"
#include "module.h"

#include "evaluator.h"
#include "nuvm.h"

#define STACK_SIZE 8192

struct nuvm_evaluator_t {
	nuvm_procedure_t* current_proc;
	nuvm_module_t* current_module;
	uint32_t code_pointer;
};

// Static prototypes.
static void
_construct(nuvm_evaluator_t* self);

static inline uint32_t
_op_global_ref(nuvm_evaluator_t*, nuvm_instruction_t);

static inline uint32_t
_op_global_set(nuvm_evaluator_t*, nuvm_instruction_t);

static inline uint32_t
_op_sva_call(nuvm_evaluator_t*, nuvm_instruction_t);

static inline uint32_t
_op_return(nuvm_evaluator_t*, nuvm_instruction_t, bool*, nuvm_value_t*);

static inline uint32_t
_op_jump(nuvm_evaluator_t*, nuvm_instruction_t);

static inline uint32_t
_op_jump_if(nuvm_evaluator_t* self, nuvm_instruction_t inst);

static inline uint32_t
_op_jump_unless(nuvm_evaluator_t* self, nuvm_instruction_t inst);

static inline void
_set_local(nuvm_evaluator_t*, uint8_t, nuvm_value_t);

static inline
nuvm_value_t _get_local(nuvm_evaluator_t*, uint8_t);

static inline void
_set_global(nuvm_evaluator_t*, uint16_t, nuvm_value_t);

static inline nuvm_value_t
_get_global(nuvm_evaluator_t*, uint16_t);


// Function implementations.
nuvm_evaluator_t* nuvm_new_evaluator() {
	size_t size = sizeof(nuvm_evaluator_t);
	nuvm_evaluator_t* self = (nuvm_evaluator_t*) nuvm_alloc_unmanaged(size);

	_construct(self);
	return self;
}
void nuvm_destroy_evaluator(nuvm_evaluator_t* self) {
	nuvm_free(self);
}

void nuvm_evaluator_setup(nuvm_evaluator_t* self, nuvm_module_t* module) {
	nuvm_value_t entry_point = nuvm_module_load_entry_point(module);
	assert(nuvm_typeof(entry_point) == NUVM_PROCEDURE_T_TYPE());
	nuvm_procedure_t* proc =
		(nuvm_procedure_t*) nuvm_unwrap_pointer(entry_point);

	self->current_proc = proc;
	self->current_module = nuvm_procedure_get_module(proc);
	self->code_pointer = nuvm_procedure_get_entry_point(proc);
}

nuvm_value_t nuvm_evaluator_run(nuvm_evaluator_t* self) {
	
	nuvm_value_t result;
	bool halt = false;
	while (! halt) {
		halt = nuvm_evaluator_step(self, &result);
	}
	return result;
}

bool nuvm_evaluator_step(nuvm_evaluator_t* self, nuvm_value_t* result) {
	uint32_t next_instruction;
	bool halt = false;
	nuvm_instruction_t inst =
		nuvm_module_fetch(self->current_module, self->code_pointer);

	switch(inst.opcode) {
		case OP_GLOBAL_REF:
			next_instruction = _op_global_ref(self, inst);
			break;
		case OP_GLOBAL_SET:
			next_instruction = _op_global_set(self, inst);
			break;
		case OP_SVA_CALL:
			next_instruction = _op_sva_call(self, inst);
			break;
		case OP_RETURN:
			next_instruction = _op_return(self, inst, &halt, result);
			break;
		case OP_JUMP:
			next_instruction = _op_jump(self, inst);
			break;
		case OP_JUMP_IF:
			next_instruction = _op_jump_if(self, inst);
			break;
		case OP_JUMP_UNLESS:
			next_instruction = _op_jump_unless(self, inst);
			break;
		default:
			assert(false);
	}
	// FIXME: Not checking the valid range for code_pointer. This could go
	// horribly wrong with malicious code, or even accidents.
	self->code_pointer = next_instruction;
	return halt;
}

// Static function implementations.
static void
_construct(nuvm_evaluator_t* self) {

}

static inline
uint32_t _op_global_ref(nuvm_evaluator_t* self, nuvm_instruction_t inst) {
	uint8_t  lindex;
	uint16_t gindex;

	nuvm_decode_op_global_ref(inst, &lindex, &gindex);

	_set_local(self, lindex, _get_global(self, gindex));
	return self->code_pointer + 1;
}

static inline
uint32_t _op_global_set(nuvm_evaluator_t* self, nuvm_instruction_t inst) {
	uint16_t gindex;
	uint8_t  lindex;

	nuvm_decode_op_global_set(inst, &gindex, &lindex);

	_set_global(self, gindex, _get_local(self, lindex));
	return self->code_pointer + 1;
}

static inline
uint32_t _op_sva_call(nuvm_evaluator_t* self, nuvm_instruction_t inst) {
	uint8_t lresult;
	uint8_t lprimitive;
	uint8_t largument;

	nuvm_decode_op_sva_call(inst, &lresult, &lprimitive, &largument);

	nuvm_value_t primitive_val = _get_local(self, lprimitive); 
	nuvm_value_t argument = _get_local(self, largument);

	assert(nuvm_typeof(primitive_val) == NUVM_PRIMITIVE_T_TYPE());

	nuvm_primitive_t* primitive =
		(nuvm_primitive_t*) nuvm_unwrap_pointer(primitive_val);
	nuvm_value_t result = nuvm_primitive_call(primitive, argument);

	_set_local(self, lresult, result);
	return self->code_pointer + 1;
}

static inline
uint32_t _op_return(nuvm_evaluator_t* self,
                    nuvm_instruction_t inst,
                    bool* halt,
					nuvm_value_t* output) {
	uint8_t lindex;
	nuvm_decode_op_return(inst, &lindex);
	nuvm_value_t value = _get_local(self, lindex);
	*halt = true;
	*output = value;
	return 0;
}

static inline uint32_t
_op_jump(nuvm_evaluator_t* self, nuvm_instruction_t inst) {
	int32_t offset;
	nuvm_decode_op_jump(inst, &offset);
	return self->code_pointer + offset;
}

static inline uint32_t
_op_jump_if(nuvm_evaluator_t* self, nuvm_instruction_t inst) {
	uint8_t lcondition;
	int16_t offset;
	nuvm_decode_op_jump_if(inst, &lcondition, &offset);
	uint32_t next_inst = self->code_pointer + 1;

	if (!nuvm_is_equal(_get_local(self, lcondition), NUVM_FALSE)) {
		next_inst = self->code_pointer + offset;
	}

	return next_inst;
}

static inline uint32_t
_op_jump_unless(nuvm_evaluator_t* self, nuvm_instruction_t inst) {
	uint8_t lcondition;
	int16_t offset;
	nuvm_decode_op_jump_unless(inst, &lcondition, &offset);
	uint32_t next_inst = self->code_pointer + 1;

	if (nuvm_is_equal(_get_local(self, lcondition), NUVM_FALSE)) {
		next_inst = self->code_pointer + offset;
	}

	return next_inst;
}


static inline
void _set_local(nuvm_evaluator_t* self, uint8_t index, nuvm_value_t val) {
	nuvm_procedure_set_local(self->current_proc, index, val);
}

static inline
nuvm_value_t _get_local(nuvm_evaluator_t* self, uint8_t index) {
	return nuvm_procedure_get_local(self->current_proc, index);
}

static inline
void _set_global(nuvm_evaluator_t* self, uint16_t index, nuvm_value_t val) {
	nuvm_module_store_register(self->current_module, index, val);
}

static inline
nuvm_value_t _get_global(nuvm_evaluator_t* self, uint16_t index) {
	return nuvm_module_load_register(self->current_module, index);
}
