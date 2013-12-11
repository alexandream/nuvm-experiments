#define AT_SUITE_NAME Module
#include "../test-suite.h"

#include <string.h>

#include "error.h"
#include "module.h"
#include "value.h"
#include "instruction.h"

static void
test_correct_construction(uint16_t reg_count, uint16_t entry);

static void
test_fetch_gives_instruction_previously_set(NModule*mod,
                                            uint32_t index,
                                            NInstruction inst);

static void
test_get_entry_point(uint16_t reg_count, uint16_t entry);

static void
test_get_register_gives_register_previously_set(uint16_t index, NValue val);

static void
test_get_invalid_instruction(uint32_t, uint32_t);

static void
test_get_invalid_register(uint16_t reg_count, uint16_t index);

static void
test_invalid_entry_point_construction(uint16_t reg_count, uint16_t entry);

static void
test_set_invalid_instruction(uint32_t code_size, uint32_t index);

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
	NModule* mod = n_module_new(0, 0, 0, &error);

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


TEST(fetch_gives_instruction_previously_set) {
	NInstruction inst0 = n_instruction(1, 2, 3, 4);
	NInstruction inst1 = n_instruction(5, 6, 7, 8);
	NInstruction inst2 = n_instruction(9, 10, 11, 12);
	NInstruction inst3 = n_instruction(13, 14, 15, 16);
	NInstruction inst4 = n_instruction(17, 18, 19, 20);

	NModule* mod = n_module_new(1, 0, 5, NULL);

	test_fetch_gives_instruction_previously_set(mod, 0, inst0);
	test_fetch_gives_instruction_previously_set(mod, 1, inst1);
	test_fetch_gives_instruction_previously_set(mod, 2, inst2);
	test_fetch_gives_instruction_previously_set(mod, 3, inst3);
	test_fetch_gives_instruction_previously_set(mod, 4, inst4);

	n_module_destroy(mod);
}


TEST(get_entry_value_gives_correct_value) {
	test_get_entry_point(5, 0);
	test_get_entry_point(5, 1);
	test_get_entry_point(5, 2);
	test_get_entry_point(5, 3);
	test_get_entry_point(5, 4);
}


TEST(get_invalid_instruction_fails) {
	test_get_invalid_instruction(0, 1);
	test_get_invalid_instruction(5, 5);
	test_get_invalid_instruction(499, 500);
	test_get_invalid_instruction(65535, 0xFFFFFFFF);
}


TEST(get_invalid_register_fails) {
	test_get_invalid_register(1, 1);
	test_get_invalid_register(1, 65535);
	test_get_invalid_register(65535, 65535);
}


TEST(get_register_gives_register_previously_set) {
	NValue v1 = n_wrap_pointer(NULL);
	NValue v2 = n_wrap_pointer(&v1);
	test_get_register_gives_register_previously_set(0, v1);
	test_get_register_gives_register_previously_set(1, v1);
	test_get_register_gives_register_previously_set(65534, v1);

	test_get_register_gives_register_previously_set(0, v2);
	test_get_register_gives_register_previously_set(1, v2);
	test_get_register_gives_register_previously_set(65534, v2);
}


TEST(invalid_entry_point_construction_fails) {
	test_invalid_entry_point_construction(1, 1);
	test_invalid_entry_point_construction(1, 65535);
	test_invalid_entry_point_construction(65535, 65535);
}


TEST(set_invalid_instruction_fails) {
	test_set_invalid_instruction(0, 1);
	test_set_invalid_instruction(5, 5);
	test_set_invalid_instruction(499, 500);
	test_set_invalid_instruction(65535, 0xFFFFFFFF);
}


TEST(set_invalid_register_fails) {
	test_set_invalid_register(1, 1);
	test_set_invalid_register(1, 65535);
	test_set_invalid_register(65535, 65535);
}



/* ----- Auxiliary functions ----- */

static void
test_correct_construction(uint16_t reg_count, uint16_t entry) {
	NError error;
	NModule* mod = n_module_new(reg_count, entry, 0, &error);

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
test_fetch_gives_instruction_previously_set(NModule*mod,
                                            uint32_t index,
                                            NInstruction inst) {
	NInstruction output;

	n_module_set_instruction(mod, index, inst, NULL);
	output = n_module_fetch(mod, index, NULL);

	EXPECT_MSG(inst.contents == output.contents,
		"Setting and subsequently fetching instruction %u "
		"yielded a different result.",
		index);
}


static void
test_get_entry_point(uint16_t reg_count, uint16_t entry) {
	NValue output;
	NValue v1 = n_wrap_pointer(NULL),
	       v2 = n_wrap_pointer((void*)0xDEADBEE0);
	NModule* mod = n_module_new(reg_count, entry, 0, NULL);
	int i;

	for (i = 0; i < reg_count; i++) {
		n_module_set_register(mod, i, v1, NULL);
	}

	n_module_set_register(mod, entry, v2, NULL);
	output = n_module_get_entry_value(mod, NULL);

	EXPECT_MSG(n_is_equal(v2, output),
		"Setting and subsequently getting entry value on register %u "
		"yielded a different result.",
		entry);
	n_module_destroy(mod);
}


static void
test_get_register_gives_register_previously_set(uint16_t index, NValue val) {
	NValue output;
	NModule* mod = n_module_new(index+1, 0, 0, NULL);

	n_module_set_register(mod, index, val, NULL);
	output = n_module_get_register(mod, index, NULL);

	EXPECT_MSG(n_is_equal(val, output),
		"Setting and subsequently getting register %u "
		"yielded a different result.",
		index);
	n_module_destroy(mod);
}


static void
test_get_invalid_register(uint16_t reg_count, uint16_t index) {
	NError error;
	NModule* mod = n_module_new(reg_count, 0, 0, NULL);

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
test_get_invalid_instruction(uint32_t inst_count, uint32_t index) {
	NError error;
	NModule* mod = n_module_new(1, 0, inst_count, NULL);

	n_module_fetch(mod, index, &error);

	ASSERT_MSG(error.code == N_E_INVALID_ARGUMENT,
		"Fetching instruction with out of bounds index %u on code "
		"segment of size %u resulted in wrong error code: %u. "
		"Expected %u.",
		index, inst_count, error.code, N_E_INVALID_ARGUMENT);

	ASSERT_MSG(strcmp(error.message, "index") == 0,
		"Fetching instruction with out of bounds index %u on code "
		"segment of size %u resulted in wrong error message: %s. "
		"Expected \"index\".",
		index, inst_count, error.message);
}


static void
test_invalid_entry_point_construction(uint16_t reg_count, uint16_t entry) {
	NError error;
	NModule* mod = n_module_new(reg_count, entry, 0, &error);

	EXPECT(mod == NULL);

	EXPECT_MSG(error.code != N_E_OK,
		"Construction of module with invalid entry point (%u, %u) "
		"failed to report any error at all.",
		reg_count, entry);

	EXPECT_MSG(error.code == N_E_INVALID_ARGUMENT,
		"Construction of module with invalid entry point (%u, %u) "
		"reported error %u, expected 'invalid argument'(%u).",
		reg_count, entry, error.code, N_E_INVALID_ARGUMENT);

	EXPECT_MSG(strcmp(error.message, "entry_point") == 0,
		"Construction of module with invalid entry point (%u, %u) "
		"reported 'invalid argument' error on wrong argument name "
		"\"%s\", expected \"entry_point\".",
		reg_count, entry, error.message);
	n_module_destroy(mod);
}


static void
test_set_invalid_instruction(uint32_t code_size, uint32_t index) {
	NError error;
	NModule* mod = n_module_new(1, 0, code_size, NULL);
	NInstruction inst;

	n_module_set_instruction(mod, index, inst, &error);

	EXPECT_MSG(error.code == N_E_INVALID_ARGUMENT,
		"Setting out of bounds instruction %u on module of code size %u "
		"yielded wrong error code %u. Expected %u.",
		index, code_size, error.code, N_E_INVALID_ARGUMENT);

	EXPECT_MSG(strcmp(error.message, "index") == 0,
		"Setting out of bounds instruction %u on module of code size %u "
		"yielded wrong error code %s. Expected \"index\".",
		index, code_size, error.message);
}

static void
test_set_invalid_register(uint16_t reg_count, uint16_t index) {
	NError error;
	NModule* mod = n_module_new(reg_count, 0, 0, NULL);
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
