#include "util/common.h"
#include "util/converter.h"

#include "instruction.h"

const uint8_t OP_GLOBAL_REF = 0x00,
              OP_GLOBAL_SET = 0x01,
              OP_SVA_CALL   = 0x02,
              OP_RETURN     = 0x03;


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

nuvm_instruction_t nuvm_op_global_set(uint16_t global, uint8_t local) {
	uint8_t arg1, arg2;
	unpack_16_to_8s(global, &arg1, &arg2);
	return _make_instruction(OP_GLOBAL_SET, arg1, arg2, local);
}
nuvm_instruction_t nuvm_op_sva_call(uint8_t prim, uint8_t out, uint8_t arg) {
	return _make_instruction(OP_SVA_CALL, prim, out, arg);
}

nuvm_instruction_t nuvm_op_return(uint8_t result) {
	return _make_instruction(OP_RETURN, result, 0, 0);
}
