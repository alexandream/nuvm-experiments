#include <string.h>
#include "../util/strings.h"
#include "../type_info.h"
#include "symbol.i.h"


#define SYMBOL_POOL_SIZE 1024

// Static prototypes.
static
void _construct(nuvm_symbol_t* self, const char* s);

static
bool _has_previous_symbol(const char* s, nuvm_symbol_t**p);

// Static variables.
static bool     _initialized = false;
static uint32_t _type_id = 0;

static nuvm_type_info_t _type_info;

static uint32_t _next_symbol;
static nuvm_symbol_t* _pool[SYMBOL_POOL_SIZE];

// Function implementations.

nuvm_symbol_t* nuvm_new_symbol(const char* source) {
	nuvm_symbol_t* self;
	bool exists = _has_previous_symbol(source, &self);
	if (!exists) {
		assert(_next_symbol < SYMBOL_POOL_SIZE);
		self = (nuvm_symbol_t*) nuvm_alloc(sizeof(nuvm_symbol_t));
		_pool[_next_symbol++] = self;

		_construct(self, source);
	}
	return self;
}

void nuvm_symbol_t_init() {
	if (!_initialized) {
		_initialized = true;
		_type_info.name = "org.nuvm.Symbol";
		_type_id = nuvm_type_info_register(&_type_info);
	}
}

uint32_t NUVM_SYMBOL_T_TYPE() {
	return _type_id;
}
// Static function implementations.
static
void _construct(nuvm_symbol_t* self, const char* s) {
	nuvm_construct_object((nuvm_object_t*) self, _type_id);
	self->size = strlen(s);
	self->string = duplicate_string(s);
}

static
bool _has_previous_symbol(const char* s, nuvm_symbol_t**p) {
	for (uint32_t i = 0; i < _next_symbol; i++) {
		if (strcmp(s, _pool[i]->string) == 0) {
			*p = _pool[i];
			return true;
		}
	}
	return false;
}

// TODO: Create destructor;
