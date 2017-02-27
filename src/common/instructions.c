#define N_INSTRUCTIONS_IGNORE_DECODE
#include "instructions.h"
#undef N_INSTRUCTIONS_IGNORE_DECODE

#include "opcodes.h"


int
n_encode_op_halt(NInstructionWord* words) {
	return 0;
}

int
n_encode_op_nop(NInstructionWord* words) {
	return 0;
}
