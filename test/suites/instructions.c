#define AT_SUITE_NAME Instructions
#include "../test-suite.h"

#include "instruction.h"

TEST(instructions_fit_32_bits) {
	EXPECT(sizeof(NInstruction) == 4);
}

TEST(pack_ordering_is_correct) {
	NInstruction inst = n_instruction(0x00, 0x01, 0x02, 0x03);
	EXPECT_MSG(inst.contents == 0x00010203,
		"Expected instruction layout to replicate 0x00010203, "
		"but got 0x%08X instead.",
		inst.contents);
}
