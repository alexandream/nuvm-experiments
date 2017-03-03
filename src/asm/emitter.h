#ifndef NHG_A_EMITTER
#define NHG_A_EMITTER

#include "common/errors.h"
#include "program.h"
#include "ostreams.h"

void
ni_emit_program(NProgram* program, NOStream* stream, NError* error);

#endif
