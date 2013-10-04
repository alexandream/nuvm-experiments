#include <string.h>

#include "../type_info.h"

#include "strings.i.h"


#include <string.h>
#include <string.h>
#include <string.h>
#include <string.h>

// Static prototypes.
static
void _construct(nuvm_string_t* self, const char* s);

// Static variables.
static bool     _initialized = false;
static uint32_t _type_id = 0;

static nuvm_type_info_t _type_info;


// Function implementations.

nuvm_string_t* nuvm_new_string(const char* source) {
	nuvm_string_t* self = 
		(nuvm_string_t*) nuvm_alloc(sizeof(nuvm_string_t));
	_construct(self, source);
	return self;
}

const char* nuvm_string_get_repr(nuvm_string_t* self) {
	return self->string;
}
bool nuvm_string_is_equal(nuvm_string_t* a, nuvm_string_t* b) {
	return (a->size == b->size && strcmp(a->string, b->string) == 0);
}
uint32_t nuvm_string_length(nuvm_string_t* self) {
	return self->size;
}

void nuvm_string_t_init() {
	if (!_initialized) {
		_initialized = true;
		_type_info.name = "org.nuvm.String";
		_type_id = nuvm_type_info_register(&_type_info);
	}
}

uint32_t NUVM_STRING_T_TYPE() {
	return _type_id;
}
// Static function implementations.
static
void _construct(nuvm_string_t* self, const char* s) {
	nuvm_construct_object((nuvm_object_t*) self, _type_id);
	self->size = strlen(s);
	self->string = strdup(s);
}

// TODO: Create destructor;
