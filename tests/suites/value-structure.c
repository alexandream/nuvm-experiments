#define AT_SUITE_NAME Value_Structure
#include "../test-suite.h"

#include "value.h"

TEST(values_are_64_bits) {
	NValue value;

	EXPECT(sizeof(value) == 8);
	EXPECT(sizeof(value.contents) == 8);
}

TEST(pointers_are_64_bits) {
	NValue value;
	EXPECT(sizeof(value.pointer) == 8);
}

TEST(components_alignment) {
	NValue v0;
	v0.contents = 0;
	v0.immediate.tags.half_tags.upper_tag  = 0xFFFF;

	EXPECT_MSG(v0.contents == 0xFFFF000000000000,
		"Expected value 0xFFFF000000000000, got 0x%016lX.\n", v0.contents);

	v0.contents = 0;
	v0.immediate.tags.half_tags.lower_tag  = 0xFFFF;
	EXPECT_MSG(v0.contents == 0x0000FFFF00000000,
		"Expected value 0x0000FFFF00000000, got 0x%016lX.\n", v0.contents);
}
