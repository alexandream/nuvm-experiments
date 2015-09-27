#ifndef NHG_A_ASSEMBLER
#define NHG_A_ASSEMBLER
#include <stdio.h>

#include "instruction.h"
#include "program.h"
#include "istreams.h"

#include "../common/errors.h"
#include "../common/polyfills/p-stdint.h"

typedef enum {
	NI_CONSTANT_STRING = 0,
	NI_CONSTANT_DOUBLE,
	NI_CONSTANT_INT32,
	NI_CONSTANT_PROCEDURE,
	NI_CONSTANT_CHARACTER
} NConstantType;


NProgram*
ni_asm_read_from_istream(NIStream* istream, NError* error);

#endif /* NHG_A_ASSEMBLER */
