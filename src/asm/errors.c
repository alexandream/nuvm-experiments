#include <stdlib.h>

#include "errors.h"
#include "../common/errors.h"



struct NErrorTypes ni_a_errors;


static void
destroy_error_with_child(NError* error) {
	NError* original_error = (NError*) error->data;
	if (original_error != NULL) {
		n_error_destroy(original_error);
		/* FIXME (#4): This should change if we change the cloning */
		free(original_error);
	}
	error->data = NULL;
}


bool
n_init_asm_errors(uint8_t* error_code, char** error_text) {
	struct NErrorTypes* e = &ni_a_errors;
	struct NReaderErrorTypes* r = &e->reader;
	struct NAssemblerErrorTypes* a = &e->assembler;

	/* General Errors */
	e->BadAllocation = n_find_error_type("nuvm.BadAllocation");

	e->UnknownError =  n_find_error_type("nuvm.UnknownError");

	e->IOError = n_register_error_type("nuvm.asm.IOError", NULL, NULL);

	e->BufferTooSmall =
		n_register_error_type("nuvm.asm.BufferTooSmall", NULL, NULL);

	e->NumberOutOfBounds =
		n_register_error_type("nuvm.asm.NumberOutOfBounds", NULL, NULL);

	/* Reader Errors */
	r->EndOfFile = n_register_error_type("nuvm.asm.reader.EOF", NULL, NULL);

	r->UnexpectedToken =
		n_register_error_type("nuvm.asm.reader.UnexpectedToken", NULL, NULL);

	r->IncompatibleRegisterType =
		n_register_error_type("nuvm.asm.reader.IncompatibleRegisterType",
		                      NULL,
		                      NULL);
	r->UnimplementedOpcode =
		n_register_error_type("nuvm.asm.reader.UnimplementedOpcode",
		                      NULL,
		                      NULL);

	r->RegisterOutOfRange =
		n_register_error_type("nuvm.asm.reader.RegisterOutOfRange",
		                      NULL,
		                      NULL);

	/* Assembler Errors */
	a->EntryPointOutOfBounds =
		n_register_error_type("nuvm.asm.assembler.EntryPointOutOfBounds",
		                      NULL,
		                      NULL);

	a->ConstantsNotFound =
		n_register_error_type("nuvm.asm.assembler.ConstantsNotFound",
		                      NULL,
		                      NULL);

	a->EmptyConstantsList =
		n_register_error_type("nuvm.asm.assembler.EmptyConstantsList",
		                      NULL,
		                      NULL);

	a->InvalidConstantSyntax =
		n_register_error_type("nuvm.asm.assembler.InvalidConstantSyntax",
		                      NULL,
		                      destroy_error_with_child);

	a->InvalidCodeSyntax =
		n_register_error_type("nuvm.asm.assembler.InvalidCodeSyntax",
		                      NULL,
		                      destroy_error_with_child);

	a->CodeNotFound =
		n_register_error_type("nuvm.asm.assembler.CodeNotFound",
		                      NULL,
		                      NULL);

	a->UnexpectedToken =
		n_register_error_type("nuvm.asm.assembler.UnexpectedToken",
		                      NULL,
		                      n_error_destroy_by_freeing);

	return true;
}
