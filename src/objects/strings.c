#include <stdlib.h>
#include <stdio.h>

#include "../type-info.h"
#include "../memory.h"

#include "strings.h"

struct NString {
	NObject parent;
	uint32_t length;
	char* characters;
};

static NType   _type;
static int32_t _type_id;

void
n_init_strings() {
	NError error;
	NTypeRegistry* registry = n_type_registry_get_default();

	n_type_init(&_type, "org.nuvm.String");

	_type_id = n_type_registry_add_type(registry, &_type, &error);
	if (error.code != N_E_OK) {
		fprintf(stderr, "Unable to register type for Strings. Aborting.\n");
		exit(1);
	}
}


bool
n_is_string(NValue val) {
	return n_typeof(val) == _type_id;
}

