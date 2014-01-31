#include <string.h>

#include "loader.h"
#include "stdlib/core.h"


NValue
n_load(const char* name, NError* error) {
	n_error_clear(error);

	if (strcmp(name, "core") != 0) {
		n_error_set(error, N_E_LOADABLE_NOT_FOUND);
		return N_UNDEFINED;
	}

	return n_load_stdlib_core();
}
