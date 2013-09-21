#define TEST_CASE_NAME "Instruction Representation"
#include "../test-case.h"

#include "instruction.h"
#include "nuvm.h"

void _test_jump_encode_decode(int32_t input);
void _test_jump_if_encode_decode(uint8_t cond, int16_t offset);
void _test_jump_unless_encode_decode(uint8_t cond, int16_t offset);


BEGIN_TEST(instructions_size_is_32bits) {
	fail_if(sizeof(nuvm_instruction_t) != 4);
} END_TEST

BEGIN_TEST(instruction_packing) {
	nuvm_instruction_t inst;
	inst.opcode = 0x00;
	inst.arg1   = 0x01;
	inst.arg2   = 0x02;
	inst.arg3   = 0x03;

	fail_if(inst.contents != 0x00010203);
} END_TEST

BEGIN_TEST(op_global_ref_encoding) {
	nuvm_instruction_t inst = nuvm_op_global_ref(0x01, 0xBEEF);
	fail_unless(inst.opcode == OP_GLOBAL_REF);

	uint8_t local;
	uint16_t global;
	nuvm_decode_op_global_ref(inst, &local, &global);

	fail_unless(local == 0x01);
	fail_unless(global == 0xBEEF);
} END_TEST


BEGIN_TEST(op_global_set_encoding) {
	nuvm_instruction_t inst = nuvm_op_global_set(0xBEEF, 0xAA);
	fail_unless(inst.opcode == OP_GLOBAL_SET);

	uint16_t global;
	uint8_t local;
	nuvm_decode_op_global_set(inst, &global, &local);

	fail_unless(global == 0xBEEF);
	fail_unless(local == 0xAA);
} END_TEST

BEGIN_TEST(op_sva_call_encoding) {
	nuvm_instruction_t inst = nuvm_op_sva_call(0x01, 0x02, 0x03);
	fail_unless(inst.opcode == OP_SVA_CALL);

	uint8_t prim, out, arg;
	nuvm_decode_op_sva_call(inst, &out, &prim,  &arg);
	fail_unless(out  == 0x01);
	fail_unless(prim == 0x02);
	fail_unless(arg  == 0x03);
} END_TEST

BEGIN_TEST(op_return_encoding) {
	nuvm_instruction_t inst = nuvm_op_return(0x01);
	fail_unless(inst.opcode == OP_RETURN);

	uint8_t result;
	nuvm_decode_op_return(inst, &result);

	fail_unless(result == 0x01);
} END_TEST


BEGIN_TEST(op_jump_encoding) {
	int32_t min = -8388607;
	int32_t mid = 1;
	int32_t max = 8388607;

	_test_jump_encode_decode(min);
	_test_jump_encode_decode(mid);
	_test_jump_encode_decode(max);
} END_TEST

BEGIN_TEST(op_jump_if_encoding) {
	int16_t min = INT16_MIN;
	int16_t mid = 1;
	int16_t max = INT16_MAX;

	_test_jump_if_encode_decode(0x01, min);
	_test_jump_if_encode_decode(0x02, mid);
	_test_jump_if_encode_decode(0x03, max);
} END_TEST

BEGIN_TEST(op_jump_unless_encoding) {
	int16_t min = INT16_MIN;
	int16_t mid = 1;
	int16_t max = INT16_MAX;

	_test_jump_unless_encode_decode(0x01, min);
	_test_jump_unless_encode_decode(0x02, mid);
	_test_jump_unless_encode_decode(0x03, max);
} END_TEST


void _test_jump_encode_decode(int32_t input) {
	nuvm_instruction_t inst = nuvm_op_jump(input);
	int32_t result;
	nuvm_decode_op_jump(inst, &result);
	fail_unless(input == result, "Expected %d, got %d.", input, result);
}
void _test_jump_if_encode_decode(uint8_t cond, int16_t offset) {
	nuvm_instruction_t inst = nuvm_op_jump_if(cond, offset);
	fail_unless(inst.opcode == OP_JUMP_IF);

	uint8_t _cond;
	int16_t _offset;
	nuvm_decode_op_jump_if(inst, &_cond, &_offset);
	fail_unless(offset == _offset,
	            "Expected Offset %d, got %d.",
	            offset,
	            _offset);
	fail_unless(cond == _cond,
	            "Expected Condition %u, got %u.",
	            cond,
	            _cond);
}

void _test_jump_unless_encode_decode(uint8_t cond, int16_t offset) {
	nuvm_instruction_t inst = nuvm_op_jump_unless(cond, offset);
	fail_unless(inst.opcode == OP_JUMP_IF);

	uint8_t _cond;
	int16_t _offset;
	nuvm_decode_op_jump_unless(inst, &_cond, &_offset);
	fail_unless(offset == _offset,
	            "Expected Offset %d, got %d.",
	            offset,
	            _offset);
	fail_unless(cond == _cond,
	            "Expected Condition %u, got %u.",
	            cond,
	            _cond);
}
