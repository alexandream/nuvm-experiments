#define SUITE_NAME Assembler
#include <string.h>

#define USE_JOINED_STRING
#include "../../test-suite.h"

#include "../common/opcodes.h"
#include "assembler.h"
#include "program.h"


TEST(read_from_istream) {
	const char* prog_lines[] = {
	    ".version 0 0 1                \n",
	    ".entry-point 0                \n",
	    ".globals-count 3              \n",
	    "                              \n",
	    ".constants                    \n",
	    "  .procedure @INIT 25         \n",
	    "  .int32 10588                \n",
	    "  .int32 42                   \n",
	    "                              \n",
	    ".code                         \n",
	    "                              \n",
	    "INIT:                         \n",
	    "  global-ref    0    1        \n",
	    "  global-ref    1    2        \n",
	    "  add 3 1 2                   \n",
	    "END:                          \n",
	    "  return 3                    \n"
	};
	char prog_buffer[EXPECTED_SIZE(prog_lines, 80)];
	const char* prog_text =
		JOINED_STRING(prog_buffer, prog_lines, sizeof(prog_lines));


	NError error = N_ERROR_INITIALIZER;
	NIStream* stream = ni_new_istream_from_string(prog_text);
	NProgram* program;

	program = ni_asm_read_from_istream(stream, &error);
	ASSERT(ERROR_OK(&error));

	ASSERT(EQ_UINT(program->major_version, 0));
	ASSERT(EQ_UINT(program->minor_version, 0));
	ASSERT(EQ_UINT(program->revision, 1));

	ASSERT(EQ_UINT(program->entry_point, 0));
	ASSERT(EQ_UINT(program->globals_count, 3));
	ASSERT(EQ_INT(program->constants_size, 3));
	ASSERT(EQ_INT(program->code_size, 4));

	{
		uint8_t const_type;
		int32_t const_int32;
		uint16_t const_uint16;
		uint16_t const_label_id;
		uint32_t const_label_def;
		NInstruction inst;

		/* Check the first constant */
		const_type = program->constants[0].type;
		const_label_id = program->constants[0].label_id;
		const_label_def = program->constants[0].label_definition;
		const_uint16 = program->constants[0].uint16;

		ASSERT(EQ_UINT(const_type, NI_CONSTANT_PROCEDURE));
		ASSERT(EQ_INT(const_label_id, 1));
		ASSERT(EQ_INT(const_label_def, 0));
		ASSERT(EQ_UINT(const_uint16, 25));

		/* Check the second constant */
		const_type = program->constants[1].type;
		const_int32 = program->constants[1].int32;

		ASSERT(EQ_INT(const_type, NI_CONSTANT_INT32));
		ASSERT(EQ_INT(const_int32, 10588));

		/* Check the third constant */
		const_type = program->constants[2].type;
		const_int32 = program->constants[2].int32;

		ASSERT(EQ_INT(const_type, NI_CONSTANT_INT32));
		ASSERT(EQ_INT(const_int32, 42));

		/* Check first instruction */
		inst = program->code[0];
		ASSERT(EQ_UINT(inst.opcode, N_OP_GLOBAL_REF));
		ASSERT(EQ_INT(inst.arg_a, 0));

		ASSERT(EQ_INT(inst.arg_b, 1));

		/* Check second instruction */
		inst = program->code[1];
		ASSERT(EQ_UINT(inst.opcode, N_OP_GLOBAL_REF));
		ASSERT(EQ_INT(inst.arg_a, 1));

		ASSERT(EQ_INT(inst.arg_b, 2));

		/* Check third instruction */
		inst = program->code[2];
		ASSERT(EQ_UINT(inst.opcode, N_OP_ADD));

		ASSERT(EQ_INT(inst.arg_a, 3));

		ASSERT(EQ_INT(inst.arg_b, 1));

		ASSERT(EQ_INT(inst.arg_c, 2));

		/* Check fourth instruction */
		inst = program->code[3];
		ASSERT(EQ_UINT(inst.opcode, N_OP_RETURN));

		ASSERT(EQ_INT(inst.arg_a, 3));
	}
}
