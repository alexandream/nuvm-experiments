#include "util/common.h"
#include "util/converter.h"

#include "instruction.h"


static inline
nuvm_instruction_t _make_instruction(uint8_t opcode,
                                     uint8_t arg1,
                                     uint8_t arg2,
                                     uint8_t arg3) {
	nuvm_instruction_t result;
	result.opcode = opcode;
	result.arg1 = arg1;
	result.arg2 = arg2;
	result.arg3 = arg3;
	return result;
}

nuvm_instruction_t nuvm_op_global_ref(uint8_t local, uint16_t global) {
	uint8_t arg2, arg3;
	unpack_16_to_8s(global, &arg2, &arg3);
	return _make_instruction(OP_GLOBAL_REF, local, arg2, arg3);	

}
void nuvm_decode_op_global_ref(nuvm_instruction_t inst,
                               uint8_t* local,
                               uint16_t* global) {
	*local = inst.arg1;
	*global = pack_8s_to_16(inst.arg2, inst.arg3);
}


nuvm_instruction_t nuvm_op_global_set(uint16_t global, uint8_t local) {
	uint8_t arg1, arg2;
	unpack_16_to_8s(global, &arg1, &arg2);
	return _make_instruction(OP_GLOBAL_SET, arg1, arg2, local);
}

void nuvm_decode_op_global_set(nuvm_instruction_t inst,
                               uint16_t* global,
                               uint8_t* local) {
	*global = pack_8s_to_16(inst.arg1, inst.arg2);
	*local = inst.arg3;
}


nuvm_instruction_t nuvm_op_sva_call(uint8_t out, uint8_t prim, uint8_t arg) {
	return _make_instruction(OP_SVA_CALL, out, prim, arg);
}

void nuvm_decode_op_sva_call(nuvm_instruction_t inst,
                             uint8_t* out,
                             uint8_t* prim,
                             uint8_t* arg) {
	*out = inst.arg1;
	*prim = inst.arg2;
	*arg = inst.arg3;
}


nuvm_instruction_t nuvm_op_return(uint8_t result) {
	return _make_instruction(OP_RETURN, result, 0, 0);
}

void nuvm_decode_op_return(nuvm_instruction_t inst, uint8_t* result) {
	*result = inst.arg1;
}


nuvm_instruction_t nuvm_op_jump(int32_t offset) {
	assert(offset >= -8388607 && offset <=  8388607);
	uint8_t high = (offset & 0x00FFFFFF) >> 16;
	uint8_t mid  = (offset & 0x0000FFFF) >> 8;
	uint8_t low  = (offset & 0x000000FF);

	return _make_instruction(OP_JUMP, high, mid, low);
}

void nuvm_decode_op_jump(nuvm_instruction_t inst, int32_t *offset) {
	int32_t result =
		inst.arg1 << 16 |
		inst.arg2 << 8  |
		inst.arg3;

	if (result & 0x00800000) {
		result |= 0xFF000000;
	}

	*offset = result;
}

nuvm_instruction_t nuvm_op_jump_if(uint8_t condition, int16_t offset) {
	uint8_t arg2, arg3;
	unpack_16_to_8s((uint16_t) offset, &arg2, &arg3);
	return _make_instruction(OP_JUMP_IF, condition, arg2, arg3);
}

void nuvm_decode_op_jump_if(nuvm_instruction_t inst,
                            uint8_t* condition,
                            int16_t* offset) {
	*condition = inst.arg1;
	*offset = pack_8s_to_16(inst.arg2, inst.arg3);
}
nuvm_instruction_t nuvm_op_jump_unless(uint8_t condition, int16_t offset) {
	uint8_t arg2, arg3;
	unpack_16_to_8s((uint16_t) offset, &arg2, &arg3);
	return _make_instruction(OP_JUMP_IF, condition, arg2, arg3);
}

void nuvm_decode_op_jump_unless(nuvm_instruction_t inst,
                            uint8_t* condition,
                            int16_t* offset) {
	*condition = inst.arg1;
	*offset = pack_8s_to_16(inst.arg2, inst.arg3);
}

nuvm_instruction_t nuvm_op_call(uint8_t out, uint8_t prim, uint8_t nargs) {
	return _make_instruction(OP_CALL, out, prim, nargs);
}
void nuvm_decode_op_call(nuvm_instruction_t inst,
                         uint8_t* out,
                         uint8_t* prim,
                         uint8_t* nargs) {
	*out = inst.arg1;
	*prim = inst.arg2;
	*nargs = inst.arg3;
}


nuvm_instruction_t nuvm_op_new_symbol(uint8_t out, uint8_t input) {
	return _make_instruction(OP_NEW_SYMBOL, out, input, 0x00);
}
void nuvm_decode_op_new_symbol(nuvm_instruction_t inst,
                               uint8_t* out,
                               uint8_t* input) {
	*out = inst.arg1;
	*input = inst.arg2;
}
nuvm_instruction_t nuvm_pack_op_arguments(uint8_t arg1,
                                          uint8_t arg2,
                                          uint8_t arg3,
                                          uint8_t arg4) {
	nuvm_instruction_t result;
	result.extra.arg1 = arg1;
	result.extra.arg2 = arg2;
	result.extra.arg3 = arg3;
	result.extra.arg4 = arg4;
	return result;
}
