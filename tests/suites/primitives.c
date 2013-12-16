#define AT_SUITE_NAME Primitives
#include "../test-suite.h"

#include <string.h>

#include "error.h"

#include "objects/primitives.h"
#include "type-info.h"
#include "value.h"
#include "memory.h"

static NValue
toggle_bool(void* data, NValue, NError* error);

static NValue
wrap_input(void* data, NValue, NError* error);


TEST(type_information_is_registered) {
	NTypeRegistry* registry = n_type_registry_get_default();
	int32_t id = -1;
	NType* type =
		n_type_registry_find_type(registry,
		                          "org.nuvm.PrimitiveProcedure",
								  &id);
	ASSERT_MSG(type != NULL,
		"Type for primitive procedures doesn't seem to be registered "
		"in default type registry.");

	ASSERT_MSG(id >= 0,
		"Type for primitive procedures has invalid id");
}


TEST(primitive_construction_works) {
	NPrimitive* primitive = n_primitive_new(wrap_input, NULL, NULL);

	EXPECT(primitive != NULL);
	EXPECT(n_is_primitive(n_wrap_pointer(primitive)));
	n_primitive_destroy(primitive);
}


TEST(execution_of_primitive_sees_data) {
	NError error;
	bool checked = false;
	NPrimitive* primitive = n_primitive_new(toggle_bool, &checked, &error);

	n_primitive_call(primitive, N_UNDEFINED, &error);

	ASSERT_MSG(checked != false,
		"Upon execution of toggle_bool primitive, checked should be updated "
		"to a non-false value but wasn't. Got checked: %d.",
		checked);

	n_primitive_call(primitive, N_UNDEFINED, &error);
	ASSERT_MSG(checked == false,
		"Upon second execution of toggle_bool primitive, checked should be "
		"updated to a false value but wasn't. Got checked: %d.",
		checked);

	n_primitive_destroy(primitive);
}


TEST(execution_of_primitive_returns_function_result) {
	void* heap_addr = n_alloc_unmanaged(sizeof(NValue));
	NPrimitive* primitive = n_primitive_new(wrap_input, heap_addr, NULL);
	NValue result = n_primitive_call(primitive, N_UNDEFINED, NULL);

	EXPECT(n_is_pointer(result));
	EXPECT(n_unwrap_pointer(result) == heap_addr);

	n_primitive_destroy(primitive);
	n_free(heap_addr);
}

/* ----- Auxiliary Functions ----- */

static NValue
toggle_bool(void* data, NValue arg, NError* error) {
	NValue result;
	bool* checked = (bool*) data;

	*checked = !(*checked);
	return result;
}


static NValue
wrap_input(void* data, NValue arg, NError* error) {
	return n_wrap_pointer(data);
}
