#define AT_SUITE_NAME Bundles

#include <string.h>

#include "../test-suite.h"

#include "objects/bundles.h"
#include "type-info.h"

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
