#define SUITE_NAME Emitter

#include "../../test-suite.h"
#include "common/errors.h"

#include "program.h"
#include "ostreams.h"
#include "emitter.h"

#include <assert.h>

#define CODE_SIZE 32
#define CONSTS_SIZE 32
#define BUFFER_SIZE 1024

static void init_program(NProgram* prog);

NConstantDescriptor G_CONSTS[CONSTS_SIZE];
NInstruction        G_CODE[CODE_SIZE];


static char BUFFER[BUFFER_SIZE];
static NOStream* OUTPUT = NULL;

SETUP {
	NError error = N_ERROR_INITIALIZER;
	OUTPUT = ni_new_ostream_on_buffer(BUFFER, BUFFER_SIZE, &error);
	assert(OUTPUT != NULL);
}


TEARDOWN {
	if (OUTPUT != NULL) {
		ni_delete_ostream(OUTPUT);
	}
}


TEST(header_emitting_is_ok) {
	NProgram prog;
	NError error = N_ERROR_INITIALIZER;
	init_program(&prog);


	prog.major_version = 0x12;
	prog.minor_version = 0x48;
	prog.revision      = 0x3C;

	prog.entry_point = 0xDEAD;
	prog.globals_count = 0xBEEF;

	ni_emit_program(&prog, OUTPUT, &error);
	ASSERT(ERROR_OK(&error));
	
	/* Check Magic Number */
	ASSERT(EQ_UINT8(BUFFER[ 0], 0x7F));
	ASSERT(EQ_UINT8(BUFFER[ 1], 0x4E));
	ASSERT(EQ_UINT8(BUFFER[ 2], 0x55));
	ASSERT(EQ_UINT8(BUFFER[ 3], 0x56));
	ASSERT(EQ_UINT8(BUFFER[ 4], 0x4D));
	ASSERT(EQ_UINT8(BUFFER[ 5], 0x12));
	ASSERT(EQ_UINT8(BUFFER[ 6], 0x48));
	ASSERT(EQ_UINT8(BUFFER[ 7], 0x3c));
	
	/* Check implementation flags. */
	ASSERT(EQ_UINT8(BUFFER[ 8], 0x00));
	ASSERT(EQ_UINT8(BUFFER[ 9], 0x00));
	ASSERT(EQ_UINT8(BUFFER[10], 0x00));
	ASSERT(EQ_UINT8(BUFFER[11], 0x00));
	ASSERT(EQ_UINT8(BUFFER[12], 0x00));
	ASSERT(EQ_UINT8(BUFFER[13], 0x00));
	ASSERT(EQ_UINT8(BUFFER[14], 0x00));
	ASSERT(EQ_UINT8(BUFFER[15], 0x00));

	/* Check Entry Point. */
	ASSERT(EQ_UINT8(BUFFER[16], 0xAD));
	ASSERT(EQ_UINT8(BUFFER[17], 0xDE));

	/* Check Globals Count. */
	ASSERT(EQ_UINT8(BUFFER[18], 0xEF));
	ASSERT(EQ_UINT8(BUFFER[19], 0xBE));
}


static void
init_program(NProgram* self) {
	int i;
	NInstruction instruction_initializer = N_INSTRUCTION_INITIALIZER;
	NConstantDescriptor constant_initializer = NI_CONSTANT_INITIALIZER;
	ni_construct_program(self);
	
	for (i = 0; i < CODE_SIZE; i++) {
		G_CODE[i] = instruction_initializer;
	}
	
	for (i = 0; i < CONSTS_SIZE; i++) {
		G_CONSTS[i] = constant_initializer;
	}

	self->code = G_CODE;
	self->constants = G_CONSTS;	
}
