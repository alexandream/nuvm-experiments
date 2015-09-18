#include "program.h"

NProgram*
ni_new_program(NError* error) {
	NProgram* program;
	n_error_reset(error);

	program = (NProgram*) malloc(sizeof(NProgram));
	/* TODO: Put proper error reporting here.
	 * Should result in a bad allocation error. */
	if (program == NULL) {
		return NULL;
	}
	program->major_version = 0;
	program->minor_version = 0;
	program->revision = 0;
	program->entry_point = 0;
	program->globals_count = 0;
	program->constants = NULL;
	program->constants_size = 0;
	program->code = NULL;
	program->code_size = 0;

	program->should_free_code = false;
	program->should_free_constants = false;

	return program;
}

void
ni_destroy_program(NProgram* self) {
	free(self);
}
