#include <stdlib.h>

#include "nuvm.h"

#include "types/primitive.h"
#include "types/procedure.h"
#include "types/strings.h"
#include "types/symbol.h"

const uint8_t nuvm_version[] = { 0, 0, 6 };

// FIXME: This should be a real managed allocator for our garbage collection
// system. Turns out we don't have one yet :)
void* nuvm_alloc(size_t size) {
	return malloc(size);
}

void* nuvm_alloc_unmanaged(size_t size) {
	return malloc(size);
}

void nuvm_free(void* ptr) {
	free(ptr);
}

void nuvm_init() {
	nuvm_value_t_init();
	nuvm_primitive_t_init();
	nuvm_procedure_t_init();
	nuvm_string_t_init();
	nuvm_symbol_t_init();
}

