#include "../type_info.h"
#include "procedure.h"

struct nuvm_procedure_t {
	nuvm_object_t parent;
	nuvm_module_t* module;
	uint32_t entry_point;
	uint8_t num_locals;
	nuvm_value_t* locals;
};

// Static prototypes.
static
void _construct(nuvm_procedure_t*, nuvm_module_t*, uint32_t, uint8_t);

// Static variables.
static bool     _initialized = false;
static uint32_t _type_id = 0;

static nuvm_type_info_t _type_info;

// Function implementations.
void nuvm_procedure_t_init() {
	if (!_initialized) {
		_initialized = true;
		_type_info.name = "org.nuvm.Procedure";
		_type_id = nuvm_type_info_register(&_type_info);
	}
}

nuvm_procedure_t* 
nuvm_new_procedure(nuvm_module_t* module,
                   uint32_t entry_point,
                   uint8_t num_locals) {
	nuvm_procedure_t* self =
		(nuvm_procedure_t*) nuvm_alloc(sizeof(nuvm_procedure_t));

	_construct(self, module, entry_point, num_locals);
	return self;
}

nuvm_module_t* nuvm_procedure_get_module(nuvm_procedure_t* self) {
	return self->module;
}

uint32_t  nuvm_procedure_get_entry_point(nuvm_procedure_t* self) {
	return self->entry_point;
}

uint8_t    nuvm_procedure_get_num_locals(nuvm_procedure_t* self) {
	return self->num_locals;
}

nuvm_value_t nuvm_procedure_get_local(nuvm_procedure_t* self, uint8_t index) {
	return self->locals[index];
}

void nuvm_procedure_set_local(nuvm_procedure_t* self,
                              uint8_t index,
                              nuvm_value_t val) {
	self->locals[index] = val;
}
size_t nuvm_procedure_t_size() {
	return sizeof(nuvm_procedure_t);
}

uint32_t NUVM_PROCEDURE_T_TYPE() {
	return _type_id;
}

// Static function implementations.
static
void _construct(nuvm_procedure_t* self,
                nuvm_module_t* module,
                uint32_t entry_point,
                uint8_t num_locals) {
	nuvm_construct_object((nuvm_object_t*) self, _type_id);

	self->module = module;
    self->entry_point = entry_point;
    self->num_locals = num_locals;
	self->locals = nuvm_alloc_unmanaged(sizeof(nuvm_value_t) * num_locals);
}

