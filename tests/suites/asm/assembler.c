#define SUITE_NAME Assembler
#include <string.h>

#include "../../test-suite.h"

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
	ASSERT(EQ_I64(id, 0));
}


TEST(repeated_first_label_comes_with_id_zero) {
	int32_t id;
	ni_asm_get_label(ASM, "hello", NULL);
	id = ni_asm_get_label(ASM, "hello", NULL);

	ASSERT(EQ_I64(id, 0));
}


TEST(repeated_labels_ignore_case) {
	int32_t id;
	ni_asm_get_label(ASM, "hello", NULL);
	id = ni_asm_get_label(ASM, "hElLo", NULL);

	ASSERT(EQ_I64(id, 0));
}


TEST(new_label_increases_id) {
	int32_t id;
	ni_asm_get_label(ASM, "first_label", NULL);
	id = ni_asm_get_label(ASM, "second_label", NULL);

	ASSERT(EQ_I64(id, 1));
}

TEST(read_from_lexer) {
	const char* prog =
	    ".version 0 0 1                \n"
	    ".entry-point 0                \n"
	    ".globals-count 0              \n"
	    "                              \n"
	    ".constants                    \n"
	    "  .procedure @INIT 25         \n"
	    "  .int32 10588                \n"
	    "  .int32 42                   \n"
	    "                              \n"
	    ".code                         \n"
	    "                              \n"
	    "INIT:                         \n"
	    "  global-ref    L:0    G:1    \n"
	    "  global-ref    L:1    G:2    \n"
	    "  add L:3 C:1 C:2             \n"
	    "  return L:3                  \n";

	NAssembler* assembler = ni_new_assembler();
	NError error = N_ERROR_INITIALIZER;
	NStream* stream = ni_new_stream_from_string(prog);
	NLexer* lexer = ni_new_lexer(stream);

	ni_asm_read_from_lexer(assembler, lexer, &error);
	ASSERT(ERROR_OK(&error));
}
