
#include "core.h"

#include "../value.h"
#include "../symbol-pool.h"

#include "../objects/bundles.h"
#include "../objects/primitives.h"

static bool loaded = false;
static NValue core_bundle;

static NValue
_to_string(void*, NValue* args, uint8_t nargs, NError*);

NValue
n_load_stdlib_core() {
	/* FIXME: Should be checking errors here. */
	NBundle* result;
	NValue identifier;
	NValue value;
	
	if (! loaded) {
		result = n_bundle_new(5, NULL);
		value = n_wrap_pointer(n_primitive_new(_to_string, NULL, NULL));
		identifier = n_symbol_pool_get_symbol("to_string", NULL);

		n_bundle_set(result, identifier, value, NULL);

		n_bundle_close(result);
		loaded = true;
		core_bundle = n_wrap_pointer(result);
	}

	return core_bundle;
}



static NValue
_to_string(void* data, NValue* args, uint8_t nargs, NError* error) {
	return n_to_string(args[0]);
}
