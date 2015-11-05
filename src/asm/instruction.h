#ifndef NHG_A_INSTRUCTION
#define NHG_A_INSTRUCTION

#include "../common/errors.h"
#include "../common/polyfills/p-stdint.h"

#define N_INSTRUCTION_INITIALIZER { 0, 0, 0, 0, NULL, 0}

typedef enum {
	NI_RT_CONSTANT = 1,
	NI_RT_GLOBAL   = 2,
	NI_RT_LOCAL    = 4
} NRegisterType;


typedef struct {
	uint8_t opcode;
	int32_t arg_a;
	int32_t arg_b;
	int32_t arg_c;
	char* argument_label;
	uint16_t argument_label_id;
	uint32_t argument_label_definition;
	int32_t argument_label_offset;
} NInstruction;



void
ni_asm_instruction_destruct(NInstruction* self);
#endif
