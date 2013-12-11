#define AT_SUITE_NAME Types
#include "../test-suite.h"

#include <string.h>

#include "error.h"

#include "type-info.h"

TEST(type_construction_sets_name) {
	NType type;
	n_type_init(&type, "foo.bar.Type");

	EXPECT_MSG(strcmp(type.name, "foo.bar.Type") == 0,
		"Expected value of \"%s\" for type.name, got \"%s\".",
		"foo.bar.Type",
		type.name);
}

TEST(type_construction_zeroes_id) {
	NType type;
	n_type_init(&type, "foo.bar.Type");

	EXPECT_MSG(type.id == 0,
		"Expected value of 0 for type.id, got %u.",
		type.id);
}
