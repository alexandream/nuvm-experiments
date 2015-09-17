#define SUITE_NAME Assembler
#include <string.h>

#define USE_JOINED_STRING
#include "../../test-suite.h"

#include "../common/opcodes.h"
#include "assembler.h"

static NAssembler* ASM = NULL;

TEARDOWN {
	if (ASM != NULL) {
		ni_destroy_assembler(ASM);
		ASM = NULL;
	}
}

SETUP {
	ASM = ni_new_assembler();
}


TEST(first_label_comes_with_id_zero) {
	int32_t id = ni_asm_get_label(ASM, "hello", NULL);
	ASSERT(EQ_INT(id, 0));
}


TEST(repeated_first_label_comes_with_id_zero) {
	int32_t id;
	ni_asm_get_label(ASM, "hello", NULL);
	id = ni_asm_get_label(ASM, "hello", NULL);

	ASSERT(EQ_INT(id, 0));
}


TEST(repeated_labels_ignore_case) {
	int32_t id;
	ni_asm_get_label(ASM, "hello", NULL);
	id = ni_asm_get_label(ASM, "hElLo", NULL);

	ASSERT(EQ_INT(id, 0));
}


TEST(new_label_increases_id) {
	int32_t id;
	ni_asm_get_label(ASM, "first_label", NULL);
	id = ni_asm_get_label(ASM, "second_label", NULL);

	ASSERT(EQ_INT(id, 1));
}

TEST(read_from_lexer) {
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
	const char* prog =
		JOINED_STRING(prog_buffer, prog_lines, sizeof(prog_lines));


	NAssembler* assembler = ni_new_assembler();
	NError error = N_ERROR_INITIALIZER;
	NIStream* stream = ni_new_istream_from_string(prog);
	NLexer* lexer = ni_new_lexer(stream);

	int32_t label_id;

	uint8_t *version, const_type;
	uint16_t entry_point, globals_count;
	int32_t constants_count, code_count;
	int64_t const_integer;
	uint16_t const_aux_integer;

	NInstruction* inst;

	ni_asm_read_from_lexer(assembler, lexer, &error);

	ASSERT(ERROR_OK(&error));

	version = nt_asm_version(assembler);
	entry_point = nt_asm_entry_point(assembler);
	globals_count = nt_asm_globals_count(assembler);
	constants_count = nt_asm_constants_count(assembler);
	code_count = nt_asm_code_count(assembler);

	ASSERT(EQ_UINT(version[0], 0));
	ASSERT(EQ_UINT(version[1], 0));
	ASSERT(EQ_UINT(version[2], 1));

	ASSERT(EQ_UINT(entry_point, 0));
	ASSERT(EQ_UINT(globals_count, 3));
	ASSERT(EQ_INT(constants_count, 3));
	ASSERT(EQ_INT(code_count, 4));

	/* Check if the INIT label exists with definition at 0 */
	label_id = ni_asm_get_label(assembler, "INIT", &error);
	ASSERT(ERROR_OK(&error));
	ASSERT(EQ_INT(nt_asm_label_definition(assembler, label_id), 0));

	/* Check if the END label exists with definition at 3 */
	label_id = ni_asm_get_label(assembler, "END", &error);
	ASSERT(ERROR_OK(&error));
	ASSERT(EQ_INT(nt_asm_label_definition(assembler, label_id), 3));

	/* Check the first constant */
	const_type = nt_asm_constant_type(assembler, 0);
	const_integer = nt_asm_constant_integer(assembler, 0);
	const_aux_integer = nt_asm_constant_aux_integer(assembler, 0);

	ASSERT(EQ_UINT(const_type, NI_CONSTANT_PROCEDURE));
	label_id = ni_asm_get_label(assembler, "INIT", NULL);
	ASSERT(EQ_INT(const_integer, label_id));
	ASSERT(EQ_UINT(const_aux_integer, 25));

	/* Check the second constant */
	const_type = nt_asm_constant_type(assembler, 1);
	const_integer = nt_asm_constant_integer(assembler, 1);
	const_aux_integer = nt_asm_constant_aux_integer(assembler, 1);

	ASSERT(EQ_INT(const_type, NI_CONSTANT_INT32));
	ASSERT(EQ_INT(const_integer, 10588));

	/* Check the third constant */
	const_type = nt_asm_constant_type(assembler, 2);
	const_integer = nt_asm_constant_integer(assembler, 2);
	const_aux_integer = nt_asm_constant_aux_integer(assembler, 2);

	ASSERT(EQ_INT(const_type, NI_CONSTANT_INT32));
	ASSERT(EQ_INT(const_integer, 42));

	/* Check first instruction */
	inst = nt_asm_instruction(assembler, 0);
	ASSERT(EQ_UINT(inst->opcode, N_OP_GLOBAL_REF));
	ASSERT(EQ_INT(inst->arg_a.type, NI_RT_LOCAL));
	ASSERT(EQ_INT(inst->arg_a.value, 0));

	ASSERT(EQ_INT(inst->arg_b.type, NI_RT_GLOBAL));
	ASSERT(EQ_INT(inst->arg_b.value, 1));

	/* Check second instruction */
	inst = nt_asm_instruction(assembler, 1);
	ASSERT(EQ_UINT(inst->opcode, N_OP_GLOBAL_REF));
	ASSERT(EQ_INT(inst->arg_a.type, NI_RT_LOCAL));
	ASSERT(EQ_INT(inst->arg_a.value, 1));

	ASSERT(EQ_INT(inst->arg_b.type, NI_RT_GLOBAL));
	ASSERT(EQ_INT(inst->arg_b.value, 2));

	/* Check third instruction */
	inst = nt_asm_instruction(assembler, 2);
	ASSERT(EQ_UINT(inst->opcode, N_OP_ADD));

	ASSERT(EQ_INT(inst->arg_a.type, NI_RT_LOCAL));
	ASSERT(EQ_INT(inst->arg_a.value, 3));

	ASSERT(EQ_INT(inst->arg_b.type, NI_RT_CONSTANT));
	ASSERT(EQ_INT(inst->arg_b.value, 1));

	ASSERT(EQ_INT(inst->arg_c.type, NI_RT_CONSTANT));
	ASSERT(EQ_INT(inst->arg_c.value, 2));

	/* Check fourth instruction */
	inst = nt_asm_instruction(assembler, 3);
	ASSERT(EQ_UINT(inst->opcode, N_OP_RETURN));

	ASSERT(EQ_INT(inst->arg_a.type, NI_RT_LOCAL));
	ASSERT(EQ_INT(inst->arg_a.value, 3));
}
