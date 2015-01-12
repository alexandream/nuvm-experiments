#ifndef __N__ASM__INSTRUCTION_H__
#define __N__ASM__INSTRUCTION_H__

#include "../common/errors.h"
#include "../common/polyfills/p-stdint.h"

#define N_INSTRUCTION_INITIALIZER { 0, {0, 0}, {0, 0}, {0, 0}, NULL, 0}

typedef struct {
	uint8_t type;
	int32_t value;
} NArgument;

typedef struct {
	uint8_t opcode;
	NArgument arg_a;
	NArgument arg_b;
	NArgument arg_c;
	char* argument_label;
	uint16_t argument_label_id;
} NInstruction;


NInstruction*
ni_asm_instruction_clone(NInstruction* instruction, NError* error);

void
ni_asm_instruction_destroy(NInstruction* self);
#endif
