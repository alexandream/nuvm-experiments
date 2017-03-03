#ifndef NHG_A_ASSEMBLER
#define NHG_A_ASSEMBLER
#include <stdio.h>

#include "instruction.h"
#include "program.h"
#include "istreams.h"

#include "../common/errors.h"
#include "../common/polyfills/p-stdint.h"

typedef struct NAssembler NAssembler;


NProgram*
ni_asm_read_from_istream(NIStream* istream, NError* error);

#endif /* NHG_A_ASSEMBLER */
