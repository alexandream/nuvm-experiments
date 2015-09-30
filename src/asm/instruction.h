#ifndef NHG_A_INSTRUCTION
#define NHG_A_INSTRUCTION

#include "../common/errors.h"
#include "../common/polyfills/p-stdint.h"

#define N_INSTRUCTION_INITIALIZER { 0, {0, 0}, {0, 0}, {0, 0}, NULL, 0}

typedef enum {
	NI_RT_CONSTANT = 1,
	NI_RT_GLOBAL   = 2,
	NI_RT_LOCAL    = 4
} NRegisterType;


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



void
ni_asm_instruction_destroy(NInstruction* self);
#endif
