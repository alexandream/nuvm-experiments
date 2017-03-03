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


TEST(const_procedure_emitting_is_ok) {
	NProgram prog;
	NError error = N_ERROR_INITIALIZER;
	NConstantDescriptor procedure;
	init_program(&prog);


	procedure.type = NI_CONSTANT_PROCEDURE;
	procedure.label_definition = 0xDEADBEEF;
	procedure.uint16 = 0xABCD;
	prog.constants[0] = procedure;
	prog.constants_size = 1;

	ni_emit_program(&prog, OUTPUT, &error);
	ASSERT(ERROR_OK(&error));

	/* Check Emitted Constant. */
	/* Opcode. */
	ASSERT(EQ_UINT8(BUFFER[28], NI_CONSTANT_PROCEDURE));
	/* Offset. */
	ASSERT(EQ_UINT8(BUFFER[29], 0xEF));
	ASSERT(EQ_UINT8(BUFFER[30], 0xBE));
	ASSERT(EQ_UINT8(BUFFER[31], 0xAD));
	ASSERT(EQ_UINT8(BUFFER[32], 0xDE));
	/* Locals. */
	ASSERT(EQ_UINT8(BUFFER[33], 0xCD));
	ASSERT(EQ_UINT8(BUFFER[34], 0xAB));
}


TEST(const_string_emitting_is_ok) {
	NProgram prog;
	NError error = N_ERROR_INITIALIZER;
	NConstantDescriptor string;
	init_program(&prog);


	string.type = NI_CONSTANT_STRING;
	string.text_length = 0x20;
	string.text = "01234567890123456789012345678901";
	prog.constants[0] = string;
	prog.constants_size = 1;

	ni_emit_program(&prog, OUTPUT, &error);
	ASSERT(ERROR_OK(&error));

	/* Check Emitted Constant. */
	/* Type. */
	ASSERT(EQ_UINT8(BUFFER[28], NI_CONSTANT_STRING));
	/* Length. */
	ASSERT(EQ_UINT8(BUFFER[29], 0x20));
	ASSERT(EQ_UINT8(BUFFER[30], 0x00));
	/* Content. */
	ASSERT(EQ_UINT8(BUFFER[31], '0'));
	ASSERT(EQ_UINT8(BUFFER[32], '1'));
	ASSERT(EQ_UINT8(BUFFER[33], '2'));
	ASSERT(EQ_UINT8(BUFFER[34], '3'));
	ASSERT(EQ_UINT8(BUFFER[35], '4'));
	ASSERT(EQ_UINT8(BUFFER[36], '5'));
	ASSERT(EQ_UINT8(BUFFER[37], '6'));
	ASSERT(EQ_UINT8(BUFFER[38], '7'));
	ASSERT(EQ_UINT8(BUFFER[39], '8'));
	ASSERT(EQ_UINT8(BUFFER[40], '9'));
	ASSERT(EQ_UINT8(BUFFER[41], '0'));
	ASSERT(EQ_UINT8(BUFFER[42], '1'));
	ASSERT(EQ_UINT8(BUFFER[43], '2'));
	ASSERT(EQ_UINT8(BUFFER[44], '3'));
	ASSERT(EQ_UINT8(BUFFER[45], '4'));
	ASSERT(EQ_UINT8(BUFFER[46], '5'));
	ASSERT(EQ_UINT8(BUFFER[47], '6'));
	ASSERT(EQ_UINT8(BUFFER[48], '7'));
	ASSERT(EQ_UINT8(BUFFER[49], '8'));
	ASSERT(EQ_UINT8(BUFFER[50], '9'));
	ASSERT(EQ_UINT8(BUFFER[51], '0'));
	ASSERT(EQ_UINT8(BUFFER[52], '1'));
	ASSERT(EQ_UINT8(BUFFER[53], '2'));
	ASSERT(EQ_UINT8(BUFFER[54], '3'));
	ASSERT(EQ_UINT8(BUFFER[55], '4'));
	ASSERT(EQ_UINT8(BUFFER[56], '5'));
	ASSERT(EQ_UINT8(BUFFER[57], '6'));
	ASSERT(EQ_UINT8(BUFFER[58], '7'));
	ASSERT(EQ_UINT8(BUFFER[59], '8'));
	ASSERT(EQ_UINT8(BUFFER[60], '9'));
	ASSERT(EQ_UINT8(BUFFER[61], '0'));
	ASSERT(EQ_UINT8(BUFFER[62], '1'));
}


TEST(const_double_emitting_is_ok) {
	NProgram prog;
	NError error = N_ERROR_INITIALIZER;
	NConstantDescriptor real;
	init_program(&prog);


	real.type = NI_CONSTANT_DOUBLE;
	real.real = 123456.789456;
	prog.constants[0] = real;
	prog.constants_size = 1;

	ni_emit_program(&prog, OUTPUT, &error);
	ASSERT(ERROR_OK(&error));

	/* Check Emitted Constant. */
	/* Type. */
	ASSERT(EQ_UINT8(BUFFER[28], NI_CONSTANT_DOUBLE));
	/* Content. */
	/* 0x40 fe 24 0c a1 9c 9d 5a */
	ASSERT(EQ_UINT8(BUFFER[29], 0x5A));
	ASSERT(EQ_UINT8(BUFFER[30], 0x9D));
	ASSERT(EQ_UINT8(BUFFER[31], 0x9C));
	ASSERT(EQ_UINT8(BUFFER[32], 0xA1));
	ASSERT(EQ_UINT8(BUFFER[33], 0x0C));
	ASSERT(EQ_UINT8(BUFFER[34], 0x24));
	ASSERT(EQ_UINT8(BUFFER[35], 0xFE));
	ASSERT(EQ_UINT8(BUFFER[36], 0x40));
}


TEST(const_int32_emitting_is_ok) {
	NProgram prog;
	NError error = N_ERROR_INITIALIZER;
	NConstantDescriptor int32;
	init_program(&prog);


	int32.type = NI_CONSTANT_INT32;
	int32.int32 = 0xDEADBEEF;
	prog.constants[0] = int32;
	prog.constants_size = 1;

	ni_emit_program(&prog, OUTPUT, &error);
	ASSERT(ERROR_OK(&error));

	/* Check Emitted Constant. */
	/* Type. */
	ASSERT(EQ_UINT8(BUFFER[28], NI_CONSTANT_INT32));
	/* Content. */
	/* 0x40 fe 24 0c a1 9c 9d 5a */
	ASSERT(EQ_UINT8(BUFFER[29], 0xEF));
	ASSERT(EQ_UINT8(BUFFER[30], 0xBE));
	ASSERT(EQ_UINT8(BUFFER[31], 0xAD));
	ASSERT(EQ_UINT8(BUFFER[32], 0xDE));
}


TEST(const_character_emitting_is_ok) {
	NProgram prog;
	NError error = N_ERROR_INITIALIZER;
	NConstantDescriptor character;
	init_program(&prog);


	character.type = NI_CONSTANT_CHARACTER;
	character.text_length = 0x01;
	character.text = "A";
	prog.constants[0] = character;
	prog.constants_size = 1;

	ni_emit_program(&prog, OUTPUT, &error);
	ASSERT(ERROR_OK(&error));

	/* Check Emitted Constant. */
	/* Type. */
	ASSERT(EQ_UINT8(BUFFER[28], NI_CONSTANT_CHARACTER));
	/* Length. */
	ASSERT(EQ_UINT8(BUFFER[29], 0x01));
	ASSERT(EQ_UINT8(BUFFER[30], 0x00));
	/* Content. */
	ASSERT(EQ_UINT8(BUFFER[31], 'A'));
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
