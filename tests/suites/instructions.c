#define AT_SUITE_NAME Instructions
#include "../test-suite.h"

#include "instruction.h"

static int32_t
encode_decode_op_jump(int32_t offset);

static void
test_op_call_encoding(uint8_t storage, uint8_t l_callable, uint8_t nargs);

static void
test_op_jump_encoding(int32_t offset);

static void
test_op_jump_if_encoding(uint8_t condition, int16_t offset);

static void
test_op_jump_unless_encoding(uint8_t condition, int16_t offset);


TEST(instructions_fit_32_bits) {
	EXPECT(sizeof(NInstruction) == 4);
}


TEST(op_bundle_close_encoding) {
	NInstruction inst = n_op_bundle_close(0x12);
	uint8_t l_bundle;

	EXPECT_MSG(inst.base.opcode == N_OP_BUNDLE_CLOSE,
		"Expected opcode %u, got %u.",
		N_OP_BUNDLE_CLOSE, inst.base.opcode);

	n_decode_bundle_close(inst, &l_bundle);
	EXPECT_MSG(l_bundle == 0x12,
		"Expected l_bundle to be 0x12, got 0x%02X.",
		l_bundle);
}


TEST(op_bundle_get_encoding) {
	NInstruction inst = n_op_bundle_get(0xFE, 0x12, 0x34);
	uint8_t dest, bundle, symbol;

	EXPECT_MSG(inst.base.opcode == N_OP_BUNDLE_GET,
		"Expected opcode %u, got %u.",
		N_OP_BUNDLE_GET, inst.base.opcode);

	n_decode_bundle_get(inst, &dest, &bundle, &symbol);
	EXPECT_MSG(dest == 0xFE,
		"Expected dest to be 0xFE, got 0x%02X.",
		dest);

	EXPECT_MSG(bundle == 0x12,
		"Expected bundle to be 0x12, got 0x%02X.",
		bundle);

	EXPECT_MSG(symbol == 0x34,
		"Expected symbol to be 0x34, got 0x%02X.",
		symbol);
}


TEST(op_bundle_set_encoding) {
	NInstruction inst = n_op_bundle_set(0x12, 0x34, 0x56);
	uint8_t bundle, symbol, value;

	EXPECT_MSG(inst.base.opcode == N_OP_BUNDLE_SET,
		"Expected opcode %u, got %u.",
		N_OP_BUNDLE_SET, inst.base.opcode);

	n_decode_bundle_set(inst, &bundle, &symbol, &value);
	EXPECT_MSG(bundle == 0x12,
		"Expected bundle to be 0x12, got 0x%02X.",
		bundle);

	EXPECT_MSG(symbol == 0x34,
		"Expected symbol to be 0x34, got 0x%02X.",
		symbol);

	EXPECT_MSG(value == 0x56,
		"Expected value to be 0x56, got 0x%02X.",
		value);
}


TEST(op_call_encoding) {
	test_op_call_encoding(0, 1, 2);
	test_op_call_encoding(4, 5, 6);
	test_op_call_encoding(255, 255, 255);
}


TEST(op_call_sva_encoding) {
	NInstruction inst = n_op_call_sva(0x01, 0x02, 0x03);
	uint8_t dest, func, arg;

	EXPECT_MSG(inst.base.opcode == N_OP_CALL_SVA,
		"Expected opcode %u, got %u.",
		N_OP_CALL_SVA, inst.base.opcode);

	n_decode_call_sva(inst, &dest, &func, &arg);
	EXPECT_MSG(dest == 0x01,
		"Expected destination to be 0x01, got 0x%02X.",
		dest);
	EXPECT_MSG(func == 0x02,
		"Expected function to be 0x02, got 0x%02X.",
		func);
	EXPECT_MSG(arg == 0x03,
		"Expected argument to be 0x03, got 0x%02X.",
		arg);
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


TEST(op_jump_correct_positive_encoding_works) {
	test_op_jump_encoding(0);
	test_op_jump_encoding(1);
	test_op_jump_encoding(8388607);
}


TEST(op_jump_correct_negative_encoding_works) {
	test_op_jump_encoding(-1);
	test_op_jump_encoding(-8388608);
}


TEST(op_jump_if_encoding) {
	test_op_jump_if_encoding(0, -32768);
	test_op_jump_if_encoding(1, -1);
	test_op_jump_if_encoding(2, 0);
	test_op_jump_if_encoding(3, 1);
	test_op_jump_if_encoding(4, 32767);
}


TEST(op_jump_positive_overflow_truncates) {
	int32_t offset;
	offset = encode_decode_op_jump(8388608);
	EXPECT(offset == 8388607);

	offset = encode_decode_op_jump(2147483647);
	EXPECT(offset == 8388607);
}


TEST(op_jump_negative_overflow_truncates) {
	int32_t offset;
	offset = encode_decode_op_jump(-8388609);
	EXPECT(offset == -8388608);

	offset = encode_decode_op_jump(-2147483648);
	EXPECT(offset == -8388608);
}


TEST(op_jump_unless_encoding) {
	test_op_jump_unless_encoding(0, -32768);
	test_op_jump_unless_encoding(1, -1);
	test_op_jump_unless_encoding(2, 0);
	test_op_jump_unless_encoding(3, 1);
	test_op_jump_unless_encoding(4, 32767);
}


TEST(op_new_bundle_encoding) {
  NInstruction inst = n_op_new_bundle(0x1234);
  uint16_t size;

  EXPECT_MSG(inst.base.opcode == N_OP_NEW_BUNDLE,
	"Expected opcode %u, got %u.",
	N_OP_NEW_BUNDLE, inst.base.opcode);

  n_decode_new_bundle(inst, &size);
  EXPECT_MSG(size == 0x1234,
	"Expected size to be 0x1234, got 0x%04X.",
	size);
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
	NInstruction args;
	EXPECT_MSG(inst.contents == 0x00010203,
		"Expected instruction layout to replicate 0x00010203, "
		"but got 0x%08X instead.",
		inst.contents);

	args.extra.arg1 = 0xff;
	args.extra.arg2 = 0xee;
	args.extra.arg3 = 0xdd;
	args.extra.arg4 = 0xcc;

	EXPECT_MSG(args.contents == 0xFFEEDDCC,
		"Expected instruction layout to replicate 0xFFEEDDCC, "
		"but got 0x%08X instead.",
		args.contents);
}

/* ----- Auxiliary Functions ----- */

static int32_t
encode_decode_op_jump(int32_t offset) {
	int32_t out_offset;
	NInstruction inst = n_op_jump(offset);

	n_decode_jump(inst, &out_offset);

	return out_offset;
}


static void
test_op_call_encoding(uint8_t storage, uint8_t l_callable, uint8_t nargs) {
	uint8_t out_storage;
	uint8_t out_l_callable;
	uint8_t out_nargs;

	NInstruction inst = n_op_call(storage, l_callable, nargs);

	EXPECT_MSG(inst.base.opcode == N_OP_CALL,
		"Call encoded with return storage %u, callable local %u and "
		"number of arguments %u, returned wrong opcode. Expected 0x%02X, "
		"got 0x%02X.",
		storage, l_callable, nargs, inst.base.opcode, N_OP_CALL);

	n_decode_call(inst, &out_storage, &out_l_callable, &out_nargs);

	EXPECT_MSG(storage == out_storage,
		"Decoding of call encoded with return storage %u "
		"yielded different return storage %u",
		storage, out_storage);
	EXPECT_MSG(l_callable == out_l_callable,
		"Decoding of call encoded with callable local %u "
		"yielded different callable local %u",
		l_callable, out_l_callable);
	EXPECT_MSG(nargs == out_nargs,
		"Decoding of call encoded with number of arguments %u "
		"yielded different number of arguments %u",
		nargs, out_nargs);
}


static void
test_op_jump_encoding(int32_t offset) {
	int32_t out_offset = encode_decode_op_jump(offset);
	EXPECT_MSG(out_offset == offset,
		"Decoding of op_jump encoded with offset %d "
		"yielded different offset %d",
		offset, out_offset);
}


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
