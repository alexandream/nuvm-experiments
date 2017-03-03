#include <stdio.h>

#include "nuvm.h"
#include "value.h"
#include "instruction.h"

#include "module.i.h"


// Static prototypes.
static void
_construct_builder(nuvm_module_builder_t* self);


// Function implementations.
/// Modules
nuvm_value_t nuvm_module_load_register(nuvm_module_t* self, uint16_t index) {
	return register_pool_t_get(&self->registers, index);
}

nuvm_value_t nuvm_module_load_entry_point(nuvm_module_t* self) {
	return nuvm_module_load_register(self, self->entry_point);
}

void nuvm_destroy_module(nuvm_module_t* self) {
	register_pool_t_destroy(&self->registers);
}

void nuvm_module_store_register(nuvm_module_t* self,
                                uint16_t index,
                                nuvm_value_t value) {
	register_pool_t_set(&self->registers, index, value);
}

nuvm_instruction_t nuvm_module_fetch(nuvm_module_t* self, uint32_t index) {
	return code_segment_t_get(&self->code_segment, index);
}


/// Module Builders
nuvm_module_builder_t* nuvm_new_module_builder() {
	size_t size = sizeof(nuvm_module_builder_t);
	nuvm_module_builder_t* self =
		(nuvm_module_builder_t*) nuvm_alloc_unmanaged(size);

	_construct_builder(self);
	return self;
}

void nuvm_destroy_module_builder(nuvm_module_builder_t* self) {
	register_stack_t_destroy(&self->registers);
	code_stack_t_destroy(&self->instructions);
}

uint16_t nuvm_module_builder_push_register(nuvm_module_builder_t* self,
                                       nuvm_value_t value) {
	uint32_t index = register_stack_t_push(&self->registers, value);
	assert(index < 0xFFFF);
	return (uint16_t) index;
}

nuvm_module_t* nuvm_new_blank_module(uint16_t num_registers,
                                     uint32_t num_instructions) {
	nuvm_module_t* self =
		(nuvm_module_t*) nuvm_alloc(sizeof(nuvm_module_t));
	
	register_pool_t registers;
	register_pool_t_init(&registers, num_registers);

	code_segment_t instructions;
	code_segment_t_init(&instructions, num_instructions);

	self->registers = registers;
	self->code_segment = instructions;
		
	return self;
}
uint32_t nuvm_module_builder_push_instruction(nuvm_module_builder_t* self,
                                              nuvm_instruction_t inst) {
	return code_stack_t_push(&self->instructions, inst);
}

nuvm_module_t* nuvm_module_builder_build(nuvm_module_builder_t* self,
                                         uint16_t entry_point) {
	nuvm_value_t* registers = register_stack_t_elements(&self->registers);
	
	nuvm_instruction_t* instructions =
		code_stack_t_elements(&self->instructions);

	uint32_t reglength = register_stack_t_size(&self->registers);
	uint32_t codelen = code_stack_t_size(&self->instructions);
	assert(reglength < 0xFFFFul);
	assert(entry_point < reglength);
	
	assert(codelen > 0);
	
	nuvm_module_t* mod = nuvm_new_blank_module(reglength, codelen);
	mod->entry_point = entry_point;

	for (int i = 0; i < reglength; i++) {
		register_pool_t_set(&mod->registers, i, registers[i]);
	}

	for (int64_t i = 0; i < codelen; i++) {
		code_segment_t_set(&mod->code_segment, i, instructions[i]);
	}

	return mod;
}

// Static function implementations.
/// Modules


/// Module Builders
static void
_construct_builder(nuvm_module_builder_t* self) {
	uint32_t initial_registers = 8;
	uint32_t initial_instructions = 256;

	register_stack_t_init(&self->registers, initial_registers);
	code_stack_t_init(&self->instructions, initial_instructions);
}

