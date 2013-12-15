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
