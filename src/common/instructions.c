#include "instructions.h"

#include "opcodes.h"


int
n_encode_op_halt(NInstructionWord* words) {
	return n_encode_instruction_8(words, N_OP_HALT, 0x00);
}

int
n_encode_op_nop(NInstructionWord* words) {
	return n_encode_instruction_8(words, N_OP_NOP, 0x00);
}
