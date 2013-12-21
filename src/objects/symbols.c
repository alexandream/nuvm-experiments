#include <stdio.h>
#include <stdlib.h>

#include "../type-info.h"

#include "symbols.h"

static NType   _type;
static int32_t _type_id;

void
n_init_symbols() {
	NError error;
	NTypeRegistry* registry = n_type_registry_get_default();

	n_type_init(&_type, "org.nuvm.Symbol");

	_type_id = n_type_registry_add_type(registry, &_type, &error);
	if (error.code != N_E_OK) {
		fprintf(stderr, "Unable to register type for Symbols. Aborting.\n");
		exit(1);
	}
}
