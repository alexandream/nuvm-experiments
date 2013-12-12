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


TEST(op_global_ref_encoding) {
	NInstruction inst = n_op_global_ref(0x01, 0xBEEF);
	uint8_t l_dest;
	uint16_t g_src;

	EXPECT_MSG(inst.base.opcode == N_OP_GLOBAL_REF,
		"Expected opcode %u, got %u.",
		N_OP_GLOBAL_REF, inst.base.opcode);

	n_decode_global_ref(inst, &l_dest, &g_src);
	EXPECT_MSG(l_dest == 0x01,
		"Expected destination to be 0x01, got 0x%02X.",
		l_dest);
	EXPECT_MSG(g_src == 0xBEEF,
		"Expected source to be 0xBEEF, got 0x%04X.",
		g_src);
}


TEST(op_global_set_encoding) {
	NInstruction inst = n_op_global_set(0xDEAD, 0xB0);
	uint16_t g_dest;
	uint8_t l_src;

	EXPECT_MSG(inst.base.opcode == N_OP_GLOBAL_SET,
		"Expected opcode %u, got %u.",
		N_OP_GLOBAL_SET, inst.base.opcode);

	n_decode_global_set(inst, &g_dest, &l_src);
	EXPECT_MSG(g_dest == 0xDEAD,
		"Expected destination to be 0xDEAD, got 0x%04X.",
		g_dest);
	EXPECT_MSG(l_src == 0xB0,
		"Expected source to be 0xB0, got 0x%02X.",
		l_src);
}


TEST(op_return_encoding) {
	NInstruction inst = n_op_return(0x28);
	uint8_t l_src;

	EXPECT_MSG(inst.base.opcode == N_OP_RETURN,
		"Expected opcode %u, got %u.",
		N_OP_RETURN, inst.base.opcode);

	n_decode_return(inst, &l_src);
	EXPECT_MSG(l_src == 0x28,
		"Expected source to be 0x28, got 0x%02X.",
		l_src);
}
