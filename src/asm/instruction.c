#include <stdlib.h>

#include "instruction.h"

#include "../common/polyfills/p-strdup.h"

NInstruction*
ni_asm_instruction_clone(NInstruction* instruction, NError* error) {
	NInstruction* result = (NInstruction*) malloc(sizeof(NInstruction));
	if (result == NULL) {
		/* FIXME: Raise a real error here. */
		error->type = n_find_error_type("nuvm.UnknownError");
		return NULL;
	}

	result->opcode = instruction->opcode;
	result->arg_a = instruction->arg_a;
	result->arg_b = instruction->arg_b;
	result->arg_c = instruction->arg_c;
	if (instruction->argument_label != NULL) {
		result->argument_label = strdup(instruction->argument_label);
	}
	else {
		result->argument_label = NULL;
	}
	result->argument_label_id = instruction->argument_label_id;
	return result;
}


void
ni_asm_instruction_destroy(NInstruction* self) {
	if (self->argument_label != NULL) {
		free(self->argument_label);
	}
}
