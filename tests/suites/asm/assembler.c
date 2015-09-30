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
	    "  global-ref    L:0    G:1    \n",
	    "  global-ref    L:1    G:2    \n",
	    "  add L:3 C:1 C:2             \n",
	    "END:                          \n",
	    "  return L:3                  \n"
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
		int64_t const_integer;
		uint16_t const_aux_integer;
		NInstruction inst;

		/* Check the first constant */
		const_type = program->constants[0].type;
		const_integer = program->constants[0].integer;
		const_aux_integer = program->constants[0].aux_integer;

		ASSERT(EQ_UINT(const_type, NI_CONSTANT_PROCEDURE));
		ASSERT(EQ_INT(const_integer, 0));
		ASSERT(EQ_UINT(const_aux_integer, 25));

		/* Check the second constant */
		const_type = program->constants[1].type;
		const_integer = program->constants[1].integer;

		ASSERT(EQ_INT(const_type, NI_CONSTANT_INT32));
		ASSERT(EQ_INT(const_integer, 10588));

		/* Check the third constant */
		const_type = program->constants[2].type;
		const_integer = program->constants[2].integer;

		ASSERT(EQ_INT(const_type, NI_CONSTANT_INT32));
		ASSERT(EQ_INT(const_integer, 42));

		/* Check first instruction */
		inst = program->code[0];
		ASSERT(EQ_UINT(inst.opcode, N_OP_GLOBAL_REF));
		ASSERT(EQ_INT(inst.arg_a.type, NI_RT_LOCAL));
		ASSERT(EQ_INT(inst.arg_a.value, 0));

		ASSERT(EQ_INT(inst.arg_b.type, NI_RT_GLOBAL));
		ASSERT(EQ_INT(inst.arg_b.value, 1));

		/* Check second instruction */
		inst = program->code[1];
		ASSERT(EQ_UINT(inst.opcode, N_OP_GLOBAL_REF));
		ASSERT(EQ_INT(inst.arg_a.type, NI_RT_LOCAL));
		ASSERT(EQ_INT(inst.arg_a.value, 1));

		ASSERT(EQ_INT(inst.arg_b.type, NI_RT_GLOBAL));
		ASSERT(EQ_INT(inst.arg_b.value, 2));

		/* Check third instruction */
		inst = program->code[2];
		ASSERT(EQ_UINT(inst.opcode, N_OP_ADD));

		ASSERT(EQ_INT(inst.arg_a.type, NI_RT_LOCAL));
		ASSERT(EQ_INT(inst.arg_a.value, 3));

		ASSERT(EQ_INT(inst.arg_b.type, NI_RT_CONSTANT));
		ASSERT(EQ_INT(inst.arg_b.value, 1));

		ASSERT(EQ_INT(inst.arg_c.type, NI_RT_CONSTANT));
		ASSERT(EQ_INT(inst.arg_c.value, 2));

		/* Check fourth instruction */
		inst = program->code[3];
		ASSERT(EQ_UINT(inst.opcode, N_OP_RETURN));

		ASSERT(EQ_INT(inst.arg_a.type, NI_RT_LOCAL));
		ASSERT(EQ_INT(inst.arg_a.value, 3));
	}
}
