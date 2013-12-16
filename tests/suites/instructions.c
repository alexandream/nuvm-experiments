#define AT_SUITE_NAME Instructions
#include "../test-suite.h"

#include "instruction.h"

static void
test_op_jump_if_encoding(uint8_t condition, int16_t offset);

static void
test_op_jump_unless_encoding(uint8_t condition, int16_t offset);


TEST(instructions_fit_32_bits) {
	EXPECT(sizeof(NInstruction) == 4);
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


TEST(op_jump_if_encoding) {
	test_op_jump_if_encoding(0, -32768);
	test_op_jump_if_encoding(1, -1);
	test_op_jump_if_encoding(2, 0);
	test_op_jump_if_encoding(3, 1);
	test_op_jump_if_encoding(4, 32767);
}


TEST(op_jump_unless_encoding) {
	test_op_jump_unless_encoding(0, -32768);
	test_op_jump_unless_encoding(1, -1);
	test_op_jump_unless_encoding(2, 0);
	test_op_jump_unless_encoding(3, 1);
	test_op_jump_unless_encoding(4, 32767);
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


TEST(pack_ordering_is_correct) {
	NInstruction inst = n_instruction(0x00, 0x01, 0x02, 0x03);
	EXPECT_MSG(inst.contents == 0x00010203,
		"Expected instruction layout to replicate 0x00010203, "
		"but got 0x%08X instead.",
		inst.contents);
}

/* ----- Auxiliary Functions ----- */

static void
test_op_jump_if_encoding(uint8_t condition, int16_t offset) {
	uint8_t out_condition;
	int16_t out_offset;

	NInstruction inst = n_op_jump_if(condition, offset);

	EXPECT_MSG(inst.base.opcode == N_OP_JUMP_IF,
		"JUMP-IF returned wrong opcode %u "
		"when built with condition %d and offset %d.",
		inst.base.opcode, condition, offset);

	n_decode_jump_if(inst, &out_condition, &out_offset);

	EXPECT_MSG(out_condition == condition,
		"JUMP-IF returned wrong opcode %u "
		"when built with condition %d and offset %d.",
		out_condition, condition, offset);
	EXPECT_MSG(out_offset == offset,
		"JUMP-IF returned wrong opcode %u "
		"when built with condition %d and offset %d.",
		out_offset, condition, offset);
}


static void
test_op_jump_unless_encoding(uint8_t condition, int16_t offset) {
	uint8_t out_condition;
	int16_t out_offset;

	NInstruction inst = n_op_jump_unless(condition, offset);

	EXPECT_MSG(inst.base.opcode == N_OP_JUMP_UNLESS,
		"JUMP-UNLESS returned wrong opcode %u "
		"when built with condition %d and offset %d.",
		inst.base.opcode, condition, offset);

	n_decode_jump_unless(inst, &out_condition, &out_offset);

	EXPECT_MSG(out_condition == condition,
		"JUMP-UNLESS returned wrong opcode %u "
		"when built with condition %d and offset %d.",
		out_condition, condition, offset);
	EXPECT_MSG(out_offset == offset,
		"JUMP-UNLESS returned wrong opcode %u "
		"when built with condition %d and offset %d.",
		out_offset, condition, offset);
}
