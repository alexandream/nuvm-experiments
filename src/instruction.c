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
n_decode_bundle_close(NInstruction inst, uint8_t *bundle) {
	*bundle = inst.base.arg1;
}


void
n_decode_bundle_get(NInstruction inst,
                    uint8_t* dest,
                    uint8_t* bundle,
                    uint8_t* symbol) {
	*dest = inst.base.arg1;
	*bundle = inst.base.arg2;
	*symbol = inst.base.arg3;
}


void
n_decode_bundle_set(NInstruction inst,
                    uint8_t* bundle,
                    uint8_t* symbol,
                    uint8_t* value) {
	*bundle = inst.base.arg1;
	*symbol = inst.base.arg2;
	*value  = inst.base.arg3;
}


void
n_decode_call(NInstruction inst,
              uint8_t* dest,
              uint8_t* callee,
              uint8_t* nargs) {
	*dest = inst.base.arg1;
	*callee = inst.base.arg2;
	*nargs = inst.base.arg3;
}


void
n_decode_call_sva(NInstruction inst,
                  uint8_t* dest,
                  uint8_t* func,
                  uint8_t* arg ) {
	*dest = inst.base.arg1;
	*func = inst.base.arg2;
	*arg  = inst.base.arg3;
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
n_decode_jump(NInstruction inst, int32_t* offset) {
	int32_t result = 0           |
	        inst.base.arg1 << 16 |
	        inst.base.arg2 << 8  |
	        inst.base.arg3;
	/* If the highest of the 24bits is set, the number was negative.
	 * So, we should fill the rest of the preceding bits to maintain that. */
	if (result & 0x00800000) {
		result |= 0xFF000000;
	}

	*offset = result;
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
n_decode_load(NInstruction inst, uint8_t* dest, uint8_t* string) {
	*dest = inst.base.arg1;
	*string = inst.base.arg2;
}


void
n_decode_new_bundle(NInstruction inst, uint8_t* dest, uint16_t* size) {
	*dest = inst.base.arg1;
	*size = pack_8s_as_16(inst.base.arg2, inst.base.arg3);
}


void
n_decode_return(NInstruction inst, uint8_t* src) {
	*src = inst.base.arg1;
}


NInstruction
n_op_bundle_close(uint8_t bundle) {
	return n_instruction(N_OP_BUNDLE_CLOSE, bundle, 0, 0);
}


NInstruction
n_op_bundle_get(uint8_t dest, uint8_t bundle, uint8_t symbol) {
	return n_instruction(N_OP_BUNDLE_GET, dest, bundle, symbol);
}


NInstruction
n_op_bundle_set(uint8_t bundle, uint8_t symbol, uint8_t value) {
	return n_instruction(N_OP_BUNDLE_SET, bundle, symbol, value);
}


NInstruction
n_op_call(uint8_t dest, uint8_t callee, uint8_t nargs) {
	return n_instruction(N_OP_CALL, dest, callee, nargs);
}


NInstruction
n_op_call_sva(uint8_t dest, uint8_t func, uint8_t arg) {
	return n_instruction(N_OP_CALL_SVA, dest, func, arg);
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
n_op_jump(int32_t offset) {
	uint8_t high, mid, low;

	if (offset > 8388607) offset = 8388607;
	if (offset < -8388608) offset = -8388608;

	high = (offset & 0x00FFFFFF) >> 16;
	mid  = (offset & 0x0000FFFF) >> 8;
	low  = (offset & 0x000000FF);

	return n_instruction(N_OP_JUMP, high, mid, low);
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
n_op_load(uint8_t dest, uint8_t string) {
	return n_instruction(N_OP_LOAD, dest, string, 0);
}


NInstruction
n_op_new_bundle(uint8_t dest, uint16_t size) {
	uint8_t arg2, arg3;
	unpack_16_to_8s(size, &arg2, &arg3);
	return n_instruction(N_OP_NEW_BUNDLE, dest, arg2, arg3);
}


NInstruction
n_op_return(uint8_t src) {
	return n_instruction(N_OP_RETURN, src, 0, 0);
}


NInstruction
n_pack_op_arguments(uint8_t arg1, uint8_t arg2, uint8_t arg3, uint8_t arg4) {
	NInstruction inst;
	inst.extra.arg1 = arg1;
	inst.extra.arg2 = arg2;
	inst.extra.arg3 = arg3;
	inst.extra.arg4 = arg4;

	return inst;
}
