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
