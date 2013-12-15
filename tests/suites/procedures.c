#define AT_SUITE_NAME Procedures
#include <string.h>

#include "../test-suite.h"

#include "objects/procedures.h"
#include "type-info.h"

static void
test_construction(NModule* mod, uint32_t entry, uint8_t nlocals);

static void
test_invalid_entry_point_construction(NModule*, uint32_t, uint8_t);

static void
test_invalid_module_construction(uint32_t entry, uint8_t nlocals);


TEST(type_information_is_registered) {
	NTypeRegistry* registry = n_type_registry_get_default();
	int32_t id = -1;
	NType* type =
		n_type_registry_find_type(registry, "org.nuvm.Procedure", &id);

	ASSERT_MSG(type != NULL,
		"Type for procedures doesn't seem to be registered "
		"in default type registry.");

	ASSERT_MSG(id >= 0, "Type for primitive procedures has invalid id");
}


TEST(correct_construction_works) {
	NModule * mod = n_module_new(1, 0, 1, NULL);

	test_construction(mod, 0, 0);
	test_construction(mod, 0, 127);
	test_construction(mod, 0, 255);
	n_module_destroy(mod);

	mod = n_module_new(1, 0, 512, NULL);
	test_construction(mod, 0, 0);
	test_construction(mod, 128, 64);
	test_construction(mod, 256, 128);
	test_construction(mod, 511, 255);
	n_module_destroy(mod);
}

TEST(invalid_entry_point_construction_fails) {
	NModule* mod = n_module_new(1, 0, 0, NULL);

	test_invalid_entry_point_construction(mod, 0, 0);
	test_invalid_entry_point_construction(mod, 0, 255);
	n_module_destroy(mod);

	mod = n_module_new(1, 0, 512, NULL);

	test_invalid_entry_point_construction(mod, 512, 0);
	test_invalid_entry_point_construction(mod, 512, 255);
	test_invalid_entry_point_construction(mod, 0xFFFFFFFF, 0);
	test_invalid_entry_point_construction(mod, 0xFFFFFFFF, 255);
	n_module_destroy(mod);
}

TEST(null_module_construction_fails) {
	test_invalid_module_construction(0, 0);
	test_invalid_module_construction(0, 255);
	test_invalid_module_construction(0xFFFFFFFF, 0);
	test_invalid_module_construction(0xFFFFFFFF, 255);
}



/* ----- Auxiliary functions. ----- */
static void
test_construction(NModule* mod, uint32_t entry, uint8_t nlocals) {
	NError error;
	NModule* out_mod;
	uint32_t out_entry;
	uint8_t  out_nlocals;
	NProcedure* proc = n_procedure_new(mod, entry, nlocals, &error);
	EXPECT_MSG(error.code == N_E_OK,
		"Procedure construction (%p, %u, %u) generated error code %u.",
		mod, entry, nlocals, error.code);

	ASSERT(proc != NULL);
	ASSERT_MSG(n_is_procedure(n_wrap_pointer(proc)),
		"Procedure constructed from module %p, with entry point %u and %u "
		"locals failed to be recognized as procedure.",
		mod, entry, nlocals);

	out_mod = n_procedure_get_module(proc);
	out_entry = n_procedure_get_entry_point(proc);
	out_nlocals = n_procedure_count_locals(proc);

	EXPECT_MSG(mod == out_mod,
		"Procedure constructed with module %p got module reported as %p.",
		mod, out_mod);
	EXPECT_MSG(entry == out_entry,
		"Procedure constructed with entry point %u got entry point "
		"reported as %u.",
		entry, out_entry);
	EXPECT_MSG(nlocals == out_nlocals,
		"Procedure constructed with locals count %u got locals count "
		"reported as %u.",
		nlocals, out_nlocals);
	n_procedure_destroy(proc);
}


static void
test_invalid_entry_point_construction(NModule* mod,
                                      uint32_t entry,
                                      uint8_t nlocals) {
	NError error;
	NProcedure* proc = n_procedure_new(mod, entry, nlocals, &error);

	EXPECT_MSG(error.code == N_E_INVALID_ARGUMENT,
		"Procedure construction with invalid entry point %u, "
		"on module with code size %u, reported wrong error code %u. "
		"Expected %u.",
		entry, n_module_get_code_size(mod),
		error.code, N_E_INVALID_ARGUMENT);

	EXPECT_MSG(strcmp(error.message, "entry_point") == 0,
		"Procedure construction with invalid entry point %u, "
		"on module with code size %u, reported wrong error msg %s. "
		"Expected \"entry_point\".",
		entry, n_module_get_code_size(mod), error.message);
	n_procedure_destroy(proc);
}


static void
test_invalid_module_construction(uint32_t entry, uint8_t nlocals) {
	NError error;
	NProcedure* proc = n_procedure_new(NULL, entry, nlocals, &error);

	EXPECT_MSG(error.code == N_E_INVALID_ARGUMENT,
		"Procedure construction with null module with entry point %u "
		"and locals size %u reported wrong error code %u. "
		"Expected %u.",
		entry, nlocals, error.code, N_E_INVALID_ARGUMENT);

	EXPECT_MSG(strcmp(error.message, "module") == 0,
		"Procedure construction with null module with entry point %u "
		"and locals size %u reported wrong error message %s. "
		"Expected \"%s\".",
		entry, nlocals, error.message, "module");
	n_procedure_destroy(proc);
}
