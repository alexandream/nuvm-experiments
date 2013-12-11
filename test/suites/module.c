#define AT_SUITE_NAME Module
#include "../test-suite.h"

#include <string.h>

#include "error.h"
#include "module.h"
#include "value.h"

static void
test_correct_construction(uint16_t reg_count, uint16_t entry);

static void
test_get_gives_register_previously_set(uint16_t index, NValue val);

static void
test_get_invalid_register(uint16_t reg_count, uint16_t index);

static void
test_invalid_entry_point_construction(uint16_t reg_count, uint16_t entry);

static void
test_set_invalid_register(uint16_t reg_count, uint16_t index);


TEST(correct_construction_works) {
	test_correct_construction(1, 0);
	test_correct_construction(2, 0);
	test_correct_construction(3, 0);
	test_correct_construction(5, 4);
	test_correct_construction(512, 511);
	test_correct_construction(1024, 1023);
	test_correct_construction(65535, 65534);
}


TEST(empty_module_construction_fails) {
	NError error;
	NModule* mod = n_module_new(0, 0, &error);

	EXPECT(mod == NULL);

	ASSERT_MSG(error.code != N_E_OK,
	           "Construction of empty module failed to report "
	           "any error at all.");
	ASSERT_MSG(error.code == N_E_INVALID_ARGUMENT,
	           "Construction of empty module failed to report "
	           "invalid argument error.");
	ASSERT_MSG(strcmp(error.message, "register_count") == 0,
	           "Construction of empty module reported invalid argument error "
	           "under wrong argument name [%s], expected \"register_count\".",
	           error.message);
}


TEST(get_register_out_of_bounds_fails) {
	test_get_invalid_register(1, 1);
	test_get_invalid_register(1, 65535);
	test_get_invalid_register(65535, 65535);
}


TEST(invalid_entry_point_construction_fails) {
	test_invalid_entry_point_construction(1, 1);
	test_invalid_entry_point_construction(1, 65535);
	test_invalid_entry_point_construction(65535, 65535);
}


TEST(get_gives_register_previously_set) {
	NValue v1 = n_wrap_pointer(NULL);
	NValue v2 = n_wrap_pointer(&v1);
	test_get_gives_register_previously_set(0, v1);
	test_get_gives_register_previously_set(1, v1);
	test_get_gives_register_previously_set(65534, v1);

	test_get_gives_register_previously_set(0, v2);
	test_get_gives_register_previously_set(1, v2);
	test_get_gives_register_previously_set(65534, v2);
}


TEST(set_register_out_of_bounds_fails) {
	test_set_invalid_register(1, 1);
	test_set_invalid_register(1, 65535);
	test_set_invalid_register(65535, 65535);
}



/* ----- Auxiliary functions ----- */

static void
test_correct_construction(uint16_t reg_count, uint16_t entry) {
	NError error;
	NModule* mod = n_module_new(reg_count, entry, &error);

	EXPECT_MSG(mod != NULL,
		"Module construction with register count %u and entry point %u "
		"returned null module.",
		reg_count, entry);

	EXPECT_MSG(error.code == N_E_OK,
		"Module construction with register count %u and entry point %u "
		"reported error code %u.",
		reg_count, entry, error.code);

	n_module_destroy(mod);
}


static void
test_get_gives_register_previously_set(uint16_t index, NValue val) {
	NValue output;
	NModule* mod = n_module_new(index+1, 0, NULL);

	n_module_set_register(mod, index, val, NULL);
	output = n_module_get_register(mod, index, NULL);

	EXPECT_MSG(n_is_equal(val, output),
		"Setting and subsequently getting register %u "
		"yielded a different result.",
		index);
}


static void
test_get_invalid_register(uint16_t reg_count, uint16_t index) {
	NError error;
	NModule* mod = n_module_new(reg_count, 0, NULL);

	n_module_get_register(mod, index, &error);

	EXPECT_MSG(error.code == N_E_INVALID_ARGUMENT,
		"Getting out of bounds register %u on module having %u "
		"registers yielded wrong error code %u. Expected %u.",
		index, reg_count, error.code, N_E_INVALID_ARGUMENT);
	EXPECT_MSG(strcmp(error.message, "index") == 0,
		"Getting out of bounds register %u on module having %u "
		"registers yielded wrong error message \"%s\". "
		"Expected \"index\".",
		index, reg_count, error.message);

	n_module_destroy(mod);
}


static void
test_invalid_entry_point_construction(uint16_t reg_count, uint16_t entry) {
	NError error;
	NModule* mod = n_module_new(reg_count, entry, &error);

	EXPECT(mod == NULL);

	ASSERT_MSG(error.code != N_E_OK,
		"Construction of module with invalid entry point (%u, %u) "
		"failed to report any error at all.",
		reg_count, entry);

	ASSERT_MSG(error.code == N_E_INVALID_ARGUMENT,
		"Construction of module with invalid entry point (%u, %u) "
		"reported error %u, expected 'invalid argument'(%u).",
		reg_count, entry, error.code, N_E_INVALID_ARGUMENT);

	ASSERT_MSG(strcmp(error.message, "entry_point") == 0,
		"Construction of module with invalid entry point (%u, %u) "
		"reported 'invalid argument' error on wrong argument name "
		"\"%s\", expected \"entry_point\".",
		reg_count, entry, error.message);
}


static void
test_set_invalid_register(uint16_t reg_count, uint16_t index) {
	NError error;
	NModule* mod = n_module_new(reg_count, 0, NULL);
	NValue value;

	n_module_set_register(mod, index, value, &error);

	EXPECT_MSG(error.code == N_E_INVALID_ARGUMENT,
		"Setting out of bounds register %u on module having %u "
		"registers yielded wrong error code %u. Expected %u.",
		index, reg_count, error.code, N_E_INVALID_ARGUMENT);
	EXPECT_MSG(strcmp(error.message, "index") == 0,
		"Setting out of bounds register %u on module having %u "
		"registers yielded wrong error message \"%s\". "
		"Expected \"index\".",
		index, reg_count, error.message);

	n_module_destroy(mod);
}
