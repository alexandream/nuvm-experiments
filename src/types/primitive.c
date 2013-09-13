#include "../type_info.h"

#include "primitive.h"

struct nuvm_primitive_t {
	nuvm_object_t parent;
	nuvm_native_function_t function;
	void* custom_data;
};

// Static prototypes.
static
void _construct_primitive(nuvm_primitive_t*,
                          nuvm_native_function_t,
                          void*);

// Static variables.
static bool     _initialized = false;
static uint32_t _type_id = 0;

static nuvm_type_info_t _type_info;

// Function implementations.

nuvm_value_t nuvm_primitive_call(nuvm_primitive_t* self) {
	return self->function(self->custom_data);
}

void nuvm_primitive_t_init() {
	if (!_initialized) {
		_initialized = true;
		_type_info.name = "org.nuvm.PrimitiveProcedure";
		_type_id = nuvm_type_info_register(&_type_info);
	}
}

nuvm_primitive_t* 
nuvm_new_primitive(nuvm_native_function_t func, void* custom_data) {
	nuvm_primitive_t* self = (nuvm_primitive_t*)
	   	nuvm_alloc(sizeof(nuvm_primitive_t));

	_construct_primitive(self, func, custom_data);
	return self;
}

size_t nuvm_primitive_t_size() {
	return sizeof(nuvm_primitive_t);
}

uint32_t NUVM_PRIMITIVE_T_TYPE() {
	return _type_id;
}

// Static function implementations.
static
void _construct_primitive(nuvm_primitive_t* self,
                         nuvm_native_function_t func,
                         void* custom_data) {
	nuvm_construct_object((nuvm_object_t*) self, _type_id);

	self->function = func;
	self->custom_data = custom_data;
}

