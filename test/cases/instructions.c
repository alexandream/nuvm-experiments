#define TEST_CASE_NAME "Instruction Representation"
#include "../test-case.h"

#include "instruction.h"
#include "nuvm.h"


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
	fail_unless(inst.arg1 == 0x01);
	fail_unless(inst.arg2 == 0xBE);
	fail_unless(inst.arg3 == 0xEF);
} END_TEST


BEGIN_TEST(op_global_set_encoding) {
	nuvm_instruction_t inst = nuvm_op_global_set(0xBEEF, 0xAA);
	fail_unless(inst.opcode == OP_GLOBAL_SET);
	fail_unless(inst.arg1 == 0xBE);
	fail_unless(inst.arg2 == 0xEF);
	fail_unless(inst.arg3 == 0xAA);
} END_TEST

BEGIN_TEST(op_sva_call_encoding) {
	nuvm_instruction_t inst = nuvm_op_sva_call(0x01, 0x02, 0x03);
	fail_unless(inst.opcode == OP_SVA_CALL);
	fail_unless(inst.arg1 == 0x01);
	fail_unless(inst.arg2 == 0x02);
	fail_unless(inst.arg3 == 0x03);
} END_TEST

BEGIN_TEST(op_global_ref_encodingx) {
	nuvm_instruction_t inst = nuvm_op_return(0x01);
	fail_unless(inst.opcode == OP_RETURN);
	fail_unless(inst.arg1 == 0x01);
} END_TEST
