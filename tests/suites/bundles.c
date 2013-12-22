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


TEST(setting_on_open_bundle_works) {
	NError error;
	NBundle* bundle = n_bundle_new(128, NULL);
	int i;
	char buf[32];
	for (i = 0; i < 128; i++) {
		NValue symbol;
		sprintf(buf, "S%03d", i);
		symbol = n_symbol_pool_get_symbol(buf);
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
	NValue symbol = n_symbol_pool_get_symbol("Wakka");

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
		symbol = n_symbol_pool_get_symbol(buf);
		n_bundle_set(bundle, symbol, n_wrap_fixnum(i), &error);
	}

	symbol = n_symbol_pool_get_symbol("Failing");
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
	NValue symbol = n_symbol_pool_get_symbol("Hello");

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
