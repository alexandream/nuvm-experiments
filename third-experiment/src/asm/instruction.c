#include <stdlib.h>

#include "errors.h"
#include "instruction.h"

#include "../common/polyfills/p-strdup.h"

void
ni_asm_instruction_destruct(NInstruction* self) {
	if (self->argument_label != NULL) {
		free(self->argument_label);
	}
}
