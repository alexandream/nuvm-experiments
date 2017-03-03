#define AT_SUITE_NAME Bundles

#include <string.h>

#include "../test-suite.h"

#include "objects/bundles.h"
#include "type-info.h"
#include "symbol-pool.h"

void
test_valid_construction(uint16_t size);



TEST(type_information_is_registered) {
	NTypeRegistry* registry = n_type_registry_get_default();
	int32_t id = -1;
	NType* type =
		n_type_registry_find_type(registry, "org.nuvm.Bundle", &id);

	ASSERT_MSG(type != NULL,
		"Type for bundles doesn't seem to be registered "
		"in default type registry.");

	ASSERT_MSG(id >= 0, "Type for bundles has invalid id.");
}


TEST(construction_of_empty_bundle_fails) {
	NError error;
	n_bundle_new(0, &error);

	ASSERT_MSG(error.code != N_E_OK,
		"Creation of empty bundle failed to report any error at all.");

	EXPECT_MSG(error.code == N_E_INVALID_ARGUMENT,
		"Creation of empty bundle reported wrong error code: "
		"expected %d, got %d.\n",
		N_E_INVALID_ARGUMENT, error.code);

	EXPECT_MSG(strcmp(error.message, "size") == 0,
		"Creation of empty bundle reported wrong error message: "
		"expected \"size\", got \"%s\".",
		error.message);
}

TEST(valid_construction_works) {
	test_valid_construction(1);
	test_valid_construction(2);
	test_valid_construction(4);
	test_valid_construction(8);
	test_valid_construction(16);
	test_valid_construction(32);
	test_valid_construction(128);
	test_valid_construction(1024);
	test_valid_construction(65535);
}


TEST(getting_on_open_bundle_fails) {
	NError error;
	NBundle* bundle = n_bundle_new(5, NULL);
	NValue symbol = n_symbol_pool_get_symbol("sym", NULL);

	n_bundle_get(bundle, symbol, &error);
	EXPECT_MSG(error.code == N_E_INVALID_STATE,
		"Getting from empty open bundle reported wrong error code. "
		"Expected %d, got %d.",
		N_E_INVALID_STATE, error.code);
	EXPECT_MSG(strcmp(error.message, "open") == 0,
		"Getting from empty open bundle reported wrong error message. "
		"Expected \"open\", got \"%s\".",
		error.message);

	n_bundle_set(bundle, symbol, N_TRUE, NULL);

	n_bundle_get(bundle, symbol, &error);
	EXPECT_MSG(error.code == N_E_INVALID_STATE,
		"Getting from open bundle reported wrong error code. "
		"Expected %d, got %d.",
		N_E_INVALID_STATE, error.code);
	EXPECT_MSG(strcmp(error.message, "open") == 0,
		"Getting from open bundle reported wrong error message. "
		"Expected \"open\", got \"%s\".",
		error.message);
}


TEST(getting_on_closed_bundle_works) {
	NError error;
	NBundle* bundle = n_bundle_new(5, NULL);
	NValue symbol;
	int i;

	n_bundle_close(bundle);
	for (i = 0; i < 5; i++) {
		char buf[20];
		sprintf(buf, "SG%03d", i);
		symbol = n_symbol_pool_get_symbol(buf, NULL);
		n_bundle_get(bundle, symbol, &error);
		EXPECT_MSG(error.code == N_E_OK,
			"Getting symbol \"%s\" on empty closed bundle reported unexpected "
			"error code %d with message \"%s\".",
			buf, error.code, error.message);
	}
}


TEST(getting_on_bundle_results_in_previously_set_value) {
	NError error;
	NBundle* bundle = n_bundle_new(5, NULL);
	NValue symbol;
	int i;

	for (i = 0; i < 5; i++) {
		char buf[20];
		sprintf(buf, "SG%03d", i);
		symbol = n_symbol_pool_get_symbol(buf, NULL);
		n_bundle_set(bundle, symbol, n_wrap_fixnum(i), NULL);
	}

	n_bundle_close(bundle);

	for (i = 4; i >= 0; i--) {
		char buf[20];
		NValue output;
		sprintf(buf, "SG%03d", i);
		symbol = n_symbol_pool_get_symbol(buf, NULL);
		output = n_bundle_get(bundle, symbol, &error);
		EXPECT_MSG(error.code == N_E_OK,
			"Getting symbol \"%s\" on closed bundle reported unexpected "
			"error code %d with message \"%s\".",
			buf, error.code, error.message);
		EXPECT_MSG(n_is_fixnum(output),
			"Getting previously set fixnum on closed bundle with symbol "
			"\"%s\" returned output that fails to be recognized as "
			"fixnum: 0x%016lX.",
			buf, output.contents);
		EXPECT_MSG(n_unwrap_fixnum(output) == i,
			"Getting previously set fixnum on closed bundle with symbol "
			"\"%s\" returned wrong value. Expected %d, got %d.",
			buf, i, n_unwrap_fixnum(output));
	}

}


TEST(setting_on_open_bundle_works) {
	NError error;
	NBundle* bundle = n_bundle_new(128, NULL);
	int i;
	char buf[32];
	for (i = 0; i < 128; i++) {
		NValue symbol;
		sprintf(buf, "S%03d", i);
		symbol = n_symbol_pool_get_symbol(buf, NULL);
		n_bundle_set(bundle, symbol, n_wrap_fixnum(i), &error);

		EXPECT_MSG(error.code == N_E_OK,
			"Setting symbol %s on open bundle reported unexpected "
			"error code %d.",
			buf, error.code);
	}
}


TEST(setting_on_closed_bundle_fails) {
	NError error;
	NBundle* bundle = n_bundle_new(1, NULL);
	NValue symbol = n_symbol_pool_get_symbol("Wakka", NULL);

	n_bundle_close(bundle);
	n_bundle_set(bundle, symbol, N_TRUE, &error);

	EXPECT_MSG(error.code == N_E_INVALID_STATE,
		"Setting value on closed bundle reported wrong error code. "
		"Expected %d, got %d.",
		N_E_INVALID_STATE, error.code);

	EXPECT_MSG(strcmp(error.message, "closed") == 0,
		"Setting value on closed bundle reported wrong error message. "
		"Expected \"closed\", got \"%s\".",
		error.message);
}


TEST(setting_on_full_bundle_fails) {
	NError error;
	NBundle* bundle = n_bundle_new(5, NULL);
	NValue symbol;
	int i;
	char buf[32];
	for (i = 0; i < 5; i++) {
		sprintf(buf, "%d", i);
		symbol = n_symbol_pool_get_symbol(buf, NULL);
		n_bundle_set(bundle, symbol, n_wrap_fixnum(i), &error);
	}

	symbol = n_symbol_pool_get_symbol("Failing", NULL);
	n_bundle_set(bundle, symbol, N_UNDEFINED, &error);
	EXPECT_MSG(error.code == N_E_INVALID_STATE,
		"Setting mapping on full bundle reported wrong code error. "
		"Expected %d, got %d.",
		N_E_INVALID_STATE, error.code);

	EXPECT_MSG(strcmp(error.message, "overflow") == 0,
		"Setting mapping on full bundle reported wrong error message. "
		"Expected \"overflow\", got \"%s\".",
		error.message);
}


TEST(setting_same_symbol_twice_works) {
	NError error;
	NBundle* bundle = n_bundle_new(1, NULL);
	NValue symbol = n_symbol_pool_get_symbol("Hello", NULL);

	n_bundle_set(bundle, symbol, n_wrap_fixnum(1), NULL);
	n_bundle_set(bundle, symbol, n_wrap_fixnum(1), &error);

	EXPECT_MSG(error.code == N_E_OK,
		"Resetting symbol \"Hello\" on bundle reported unexpected "
		"error code %d with error message \"%s\".",
		error.code, error.message);
}

/* ----- Auxiliary functions ----- */

void
test_valid_construction(uint16_t size) {
	NError error;
	NBundle* bundle = n_bundle_new(size, &error);

	ASSERT(bundle != NULL);

	EXPECT_MSG(error.code == N_E_OK,
		"Construction of bundle of size %u reported unexpected error %d.",
		error.code);

	EXPECT_MSG(size == n_bundle_size(bundle),
		"Construction of bundle of size %u reported wrong size %u.",
		size, n_bundle_size(bundle));
}
