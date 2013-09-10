#include "nuvm.h"
#include "value.h"
#include "module.h"

#define ARRAY_TYPE_NAME     register_pool_t
#define ARRAY_CONTENTS_TYPE nuvm_value_t
#define ARRAY_ALLOCATOR     nuvm_alloc_unmanaged
#define ARRAY_DEALLOCATOR   nuvm_free
#include "util/array.h"

#define STACK_TYPE_NAME     register_stack_t
#define STACK_CONTENTS_TYPE nuvm_value_t
#define STACK_ALLOCATOR     nuvm_alloc_unmanaged
#define STACK_DEALLOCATOR   nuvm_free
#include "util/resizable_stack.h"


struct nuvm_module_t {
	uint16_t entry_point;
	register_pool_t registers;
};


struct nuvm_module_builder_t {
	register_stack_t registers;
};

// Static prototypes.
static void
_construct_module(nuvm_module_t*, uint16_t, register_pool_t*);

static nuvm_module_t*
_new_module(uint16_t, register_pool_t* registers);

static void
_construct_builder(nuvm_module_builder_t* self);
// Static variables.


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
}

uint32_t nuvm_module_builder_push_register(nuvm_module_builder_t* self,
                                       nuvm_value_t value) {
	return register_stack_t_push(&self->registers, value);
}

nuvm_module_t* nuvm_module_builder_build(nuvm_module_builder_t* self,
                                         uint16_t entry_point) {
	nuvm_value_t* registers = register_stack_t_elements(&self->registers);
	uint32_t reglength = register_stack_t_size(&self->registers);
	
	assert(reglength < 0xFFFFul);
	assert(entry_point < reglength);

	register_pool_t regpool;
	register_pool_t_init(&regpool, reglength);
	
	for (int i = 0; i < reglength; i++) {
		register_pool_t_set(&regpool, i, registers[i]);
	}
	return _new_module(entry_point, &regpool);
}

// Static function implementations.
/// Modules
static nuvm_module_t*
_new_module(uint16_t entry_point, register_pool_t* registers) {
	nuvm_module_t* self =
		(nuvm_module_t*) nuvm_alloc(sizeof(nuvm_module_t));
	
	_construct_module(self, entry_point, registers);
	return self;
}

static void
_construct_module(nuvm_module_t* self,
                  uint16_t entry_point,
                  register_pool_t* registers) {
	self->entry_point = entry_point;
	self->registers = *registers;
}

/// Module Builders
static void
_construct_builder(nuvm_module_builder_t* self) {
	uint32_t initial_registers = 8;
	register_stack_t_init(&self->registers, initial_registers);
}

