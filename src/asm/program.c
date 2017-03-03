#include "program.h"
#include "errors.h"
NProgram*
ni_new_program(NError* error) {
	NProgram* program;

	program = (NProgram*) malloc(sizeof(NProgram));
	if (program == NULL) {
		n_error_set(error, ni_a_errors.BadAllocation, NULL);
		return NULL;
	}

	ni_construct_program(program);

	return program;
}

void
ni_construct_program(NProgram* program) {
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
}

void
ni_destroy_program(NProgram* self) {
	free(self);
}
