#ifndef __NUVM__INSTRUCTION_H__
#define __NUVM__INSTRUCTION_H__

#include "util/types.h"

typedef union NInstruction NInstruction;

union NInstruction {
	uint32_t contents;
	struct {
		uint8_t arg3;
		uint8_t arg2;
		uint8_t arg1;
		uint8_t opcode;
	} base;
};

NInstruction
n_instruction(uint8_t opcode, uint8_t arg1, uint8_t arg2, uint8_t arg3);
#endif
