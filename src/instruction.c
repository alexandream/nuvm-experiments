#include "util/converter.h"

#include "instruction.h"

NInstruction
n_instruction(uint8_t opcode, uint8_t arg1, uint8_t arg2, uint8_t arg3) {
	NInstruction result;
	result.base.opcode = opcode;
	result.base.arg1   = arg1;
	result.base.arg2   = arg2;
	result.base.arg3   = arg3;
	return result;
}


void
n_decode_global_ref(NInstruction inst, uint8_t* dest, uint16_t* src) {
	*dest = inst.base.arg1;
	*src = pack_8s_as_16(inst.base.arg2, inst.base.arg3);
}


void
n_decode_global_set(NInstruction inst, uint16_t* dest, uint8_t* src) {
	*dest = pack_8s_as_16(inst.base.arg1, inst.base.arg2);
	*src = inst.base.arg3;
}


void
n_decode_jump_if(NInstruction inst, uint8_t* cond, int16_t* offset) {
	*offset = (int16_t) pack_8s_as_16(inst.base.arg2, inst.base.arg3);
	*cond = inst.base.arg1;
}


void
n_decode_jump_unless(NInstruction inst, uint8_t* cond, int16_t* offset) {
	*offset = (int16_t) pack_8s_as_16(inst.base.arg2, inst.base.arg3);
	*cond = inst.base.arg1;
}


void
n_decode_return(NInstruction inst, uint8_t* src) {
	*src = inst.base.arg1;
}


NInstruction
n_op_global_ref(uint8_t dest, uint16_t src) {
	uint8_t arg2, arg3;
	unpack_16_to_8s(src, &arg2, &arg3);
	return n_instruction(N_OP_GLOBAL_REF, dest, arg2, arg3);
}


NInstruction
n_op_global_set(uint16_t dest, uint8_t src) {
	uint8_t arg1, arg2;
	unpack_16_to_8s(dest, &arg1, &arg2);
	return n_instruction(N_OP_GLOBAL_SET, arg1, arg2, src);
}


NInstruction
n_op_jump_if(uint8_t cond, int16_t offset) {
	uint8_t arg2, arg3;
	unpack_16_to_8s((uint16_t)offset, &arg2, &arg3);
	return n_instruction(N_OP_JUMP_IF, cond, arg2, arg3);

}


NInstruction
n_op_jump_unless(uint8_t cond, int16_t offset) {
	uint8_t arg2, arg3;
	unpack_16_to_8s((uint16_t)offset, &arg2, &arg3);
	return n_instruction(N_OP_JUMP_UNLESS, cond, arg2, arg3);

}


NInstruction
n_op_return(uint8_t src) {
	return n_instruction(N_OP_RETURN, src, 0, 0);
}
