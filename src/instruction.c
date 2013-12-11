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
