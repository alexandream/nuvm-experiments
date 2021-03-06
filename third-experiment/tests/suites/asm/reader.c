#define SUITE_NAME Reader

#include "../../test-suite.h"
#include "common/errors.h"
#include "common/opcodes.h"

#include "reader.h"
#include "lexer.h"

#include "istreams.h"

static NLexer* LEXER = NULL;

static NLexer*
WITH_INPUT(const char* input);

static void
END_INPUT();

TEARDOWN {
	END_INPUT();
}


TEST(read_version_recognizes_input) {
	uint8_t major, minor, revision;
	NError error = N_ERROR_INITIALIZER;

	NLexer* lexer = WITH_INPUT(".version 5 200 44");

	ni_read_version(lexer, &major, &minor, &revision, &error);
	ASSERT(ERROR_OK(&error));
	ASSERT(EQ_UINT(major, 5));
	ASSERT(EQ_UINT(minor, 200));
	ASSERT(EQ_UINT(revision, 44));
}


TEST(read_version_rejects_incomplete_input) {
	uint8_t a,b,c;
	const char* error_name = "nuvm.asm.reader.EOF";
	NError error = N_ERROR_INITIALIZER;

	const char* inputs[] = {
		".version 5 200",
		".version 5",
		".version",
		NULL
	};

	int i = 0;
	while (inputs[i]) {
		NLexer* lexer = WITH_INPUT(inputs[i]);
		ni_read_version(lexer, &a, &b, &c, &error);
		ASSERT(HAS_ERROR(&error, error_name));
		i++;
	}
}

TEST(read_version_rejects_values_out_of_range) {
	uint8_t a,b,c;
	const char* range_error = "nuvm.asm.NumberOutOfBounds";
	NError error = N_ERROR_INITIALIZER;

	const char* inputs[] = {
		".version 0 0 256",
		".version 0 0  -1",
		".version 0 256 0",
		".version 0  -1 0",
		".version 256 0 0",
		".version  -1 0 0",
		NULL
	};

	int i = 0;
	while (inputs[i]) {
		NLexer* lexer = WITH_INPUT(inputs[i]);
		ni_read_version(lexer, &a, &b, &c, &error);
		ASSERT(HAS_ERROR(&error, range_error));
		i++;
	}
}


TEST(read_entry_point_recognizes_input) {
	uint16_t entry_point;
	NError error = N_ERROR_INITIALIZER;

	NLexer* lexer = WITH_INPUT(".entry-point 10588");

	ni_read_entry_point(lexer, &entry_point, &error);
	ASSERT(ERROR_OK(&error));
	ASSERT(EQ_UINT(entry_point, 10588));
}


TEST(read_globals_count_recognizes_input) {
	uint16_t num_globals;
	NError error = N_ERROR_INITIALIZER;

	NLexer* lexer = WITH_INPUT(".globals-count 65530");

	ni_read_globals_count(lexer, &num_globals, &error);
	ASSERT(ERROR_OK(&error));
	ASSERT(EQ_UINT(num_globals, 65530));
}


TEST(read_string_constant_recognizes_input) {
	char* value;
	size_t length;
	NError error = N_ERROR_INITIALIZER;

	NLexer* lexer = WITH_INPUT(".string \"hello, world\"");

	ni_read_string_constant(lexer, &value, &length, &error);
	ASSERT(ERROR_OK(&error));
	ASSERT(EQ_UINT(length, 12));
	ASSERT(EQ_STR(value, "hello, world"));;
}


TEST(read_string_constant_recognizes_input_with_escape_quote) {
	char* value;
	size_t length;
	NError error = N_ERROR_INITIALIZER;

	NLexer* lexer = WITH_INPUT(".string \"hello,\\\" world\"");

	ni_read_string_constant(lexer, &value, &length, &error);
	ASSERT(ERROR_OK(&error));
	ASSERT(EQ_UINT(length, 13));
	ASSERT(EQ_STR(value, "hello,\" world"));;
}


TEST(read_double_constant_recognizes_input) {
	double value;
	NError error = N_ERROR_INITIALIZER;

	NLexer* lexer = WITH_INPUT(".double 3.14159");

	ni_read_double_constant(lexer, &value, &error);
	ASSERT(ERROR_OK(&error));
	ASSERT(EQ_DBL(value, 3.14159));;
}


TEST(read_character_constant_recognizes_input) {
	char* value;
	size_t length;
	NError error = N_ERROR_INITIALIZER;

	NLexer* lexer = WITH_INPUT(".character \"a\"");

	ni_read_character_constant(lexer, &value, &length, &error);
	ASSERT(ERROR_OK(&error));
	ASSERT(EQ_UINT(length, 1));
	ASSERT(EQ_STR(value, "a"));;
}


TEST(read_procedure_constant_recognizes_input) {
	char* label_name;
	uint16_t num_locals;
	NError error = N_ERROR_INITIALIZER;

	NLexer* lexer = WITH_INPUT(".procedure @main 128");

	ni_read_procedure_constant(lexer, &label_name, &num_locals, &error);
	ASSERT(ERROR_OK(&error));
	ASSERT(EQ_STR(label_name, "main"));;
	ASSERT(EQ_UINT(num_locals, 128));
}


TEST(read_int32_constant_recognizes_input) {
	int32_t value;
	NError error = N_ERROR_INITIALIZER;

	NLexer* lexer = WITH_INPUT(".int32 -1050088");

	ni_read_int32_constant(lexer, &value, &error);
	ASSERT(ERROR_OK(&error));
	ASSERT(EQ_INT(value, -1050088L));
}


TEST(read_instruction_recognizes_arith_rel_operations) {
	NInstruction ins = N_INSTRUCTION_INITIALIZER;
	NError error = N_ERROR_INITIALIZER;

	NLexer* lexer = WITH_INPUT("add 1 2 3");
	ni_read_instruction(lexer, &ins, &error);

	ASSERT(ERROR_OK(&error));
	ASSERT(EQ_UINT(ins.opcode, N_OP_ADD));
	ASSERT(EQ_INT(ins.arg_a, 1));
	ASSERT(EQ_INT(ins.arg_b, 2));
	ASSERT(EQ_INT(ins.arg_c, 3));
}


TEST(read_instruction_recognizes_logical_or) {
	NInstruction ins = N_INSTRUCTION_INITIALIZER;
	NError error = N_ERROR_INITIALIZER;

	NLexer* lexer = WITH_INPUT("or 1 2 3");
	ni_read_instruction(lexer, &ins, &error);

	ASSERT(ERROR_OK(&error));
	ASSERT(EQ_UINT(ins.opcode, N_OP_OR));
	ASSERT(EQ_INT(ins.arg_a, 1));
	ASSERT(EQ_INT(ins.arg_b, 2));
	ASSERT(EQ_INT(ins.arg_c, 3));
}


TEST(read_instruction_recognizes_move_in) {
	NInstruction ins = N_INSTRUCTION_INITIALIZER;
	NError error = N_ERROR_INITIALIZER;

	NLexer* lexer = WITH_INPUT("move-in 1 300");
	ni_read_instruction(lexer, &ins, &error);

	ASSERT(ERROR_OK(&error));
	ASSERT(EQ_UINT(ins.opcode, N_OP_MOVE_IN));
	ASSERT(EQ_INT(ins.arg_a, 1));
	ASSERT(EQ_INT(ins.arg_b, 300));
}


TEST(read_instruction_recognizes_move_out) {
	NInstruction ins = N_INSTRUCTION_INITIALIZER;
	NError error = N_ERROR_INITIALIZER;

	NLexer* lexer = WITH_INPUT("move-out 300 1");
	ni_read_instruction(lexer, &ins, &error);

	ASSERT(ERROR_OK(&error));
	ASSERT(EQ_UINT(ins.opcode, N_OP_MOVE_OUT));
	ASSERT(EQ_INT(ins.arg_a, 300));
	ASSERT(EQ_INT(ins.arg_b, 1));
}

TEST(read_instruction_recognizes_logical_not) {
	NInstruction ins = N_INSTRUCTION_INITIALIZER;
	NError error = N_ERROR_INITIALIZER;

	NLexer* lexer = WITH_INPUT("not 255 12345");
	ni_read_instruction(lexer, &ins, &error);

	ASSERT(ERROR_OK(&error));
	ASSERT(EQ_UINT(ins.opcode, N_OP_NOT));
	ASSERT(EQ_INT(ins.arg_a, 255));
	ASSERT(EQ_INT(ins.arg_b, 12345));
}


TEST(read_instruction_recognizes_global_ref) {
	NInstruction ins = N_INSTRUCTION_INITIALIZER;
	NError error = N_ERROR_INITIALIZER;

	NLexer* lexer = WITH_INPUT("global-ref 255 12345");
	ni_read_instruction(lexer, &ins, &error);

	ASSERT(ERROR_OK(&error));
	ASSERT(EQ_UINT(ins.opcode, N_OP_GLOBAL_REF));
	ASSERT(EQ_INT(ins.arg_a, 255));
	ASSERT(EQ_INT(ins.arg_b, 12345));
}


TEST(read_instruction_recognizes_global_set) {
	NInstruction ins = N_INSTRUCTION_INITIALIZER;
	NError error = N_ERROR_INITIALIZER;

	NLexer* lexer = WITH_INPUT("global-set 12255 123");
	ni_read_instruction(lexer, &ins, &error);

	ASSERT(ERROR_OK(&error));
	ASSERT(EQ_UINT(ins.opcode, N_OP_GLOBAL_SET));
	ASSERT(EQ_INT(ins.arg_a, 12255));
	ASSERT(EQ_INT(ins.arg_b, 123));
}


TEST(read_instruction_recognizes_const_ref) {
	NInstruction ins = N_INSTRUCTION_INITIALIZER;
	NError error = N_ERROR_INITIALIZER;

	NLexer* lexer = WITH_INPUT("const-ref 255 12345");
	ni_read_instruction(lexer, &ins, &error);

	ASSERT(ERROR_OK(&error));
	ASSERT(EQ_UINT(ins.opcode, N_OP_CONST_REF));
	ASSERT(EQ_INT(ins.arg_a, 255));
	ASSERT(EQ_INT(ins.arg_b, 12345));
}


TEST(read_instruction_recognizes_load_bool_false) {
	NInstruction ins = N_INSTRUCTION_INITIALIZER;
	NError error = N_ERROR_INITIALIZER;

	NLexer* lexer = WITH_INPUT("load-bool 127 0");
	ni_read_instruction(lexer, &ins, &error);

	ASSERT(ERROR_OK(&error));
	ASSERT(EQ_UINT(ins.opcode, N_OP_LOAD_BOOL));
	ASSERT(EQ_INT(ins.arg_a, 127));
	ASSERT(EQ_INT(ins.arg_b, 0));
}


TEST(read_instruction_recognizes_load_bool_true) {
	NInstruction ins = N_INSTRUCTION_INITIALIZER;
	NError error = N_ERROR_INITIALIZER;

	NLexer* lexer = WITH_INPUT("load-bool 127 1");
	ni_read_instruction(lexer, &ins, &error);

	ASSERT(ERROR_OK(&error));
	ASSERT(EQ_UINT(ins.opcode, N_OP_LOAD_BOOL));
	ASSERT(EQ_INT(ins.arg_a, 127));
	ASSERT(EQ_INT(ins.arg_b, 1));
}


TEST(read_instruction_rejects_load_bool_other) {
	NInstruction ins = N_INSTRUCTION_INITIALIZER;
	NError error = N_ERROR_INITIALIZER;

	NLexer* lexer = WITH_INPUT("load-bool 127 3");
	ni_read_instruction(lexer, &ins, &error);
	ASSERT(HAS_ERROR(&error, "nuvm.asm.reader.RegisterOutOfRange"));
}


TEST(read_instruction_recognizes_jump) {
	NInstruction ins = N_INSTRUCTION_INITIALIZER;
	NError error = N_ERROR_INITIALIZER;

	NLexer* lexer = WITH_INPUT("jump @dest");
	ni_read_instruction(lexer, &ins, &error);
	ASSERT(ERROR_OK(&error));
	ASSERT(EQ_UINT(ins.opcode, N_OP_JUMP));
	ASSERT(EQ_STR(ins.argument_label, "dest"));
}


TEST(read_instruction_recognizes_jump_if) {
	NInstruction ins = N_INSTRUCTION_INITIALIZER;
	NError error = N_ERROR_INITIALIZER;

	NLexer* lexer = WITH_INPUT("jump-if 127 @somewhere");
	ni_read_instruction(lexer, &ins, &error);

	ASSERT(ERROR_OK(&error));
	ASSERT(EQ_UINT(ins.opcode, N_OP_JUMP_IF));
	ASSERT(EQ_INT(ins.arg_a, 127));
	ASSERT(EQ_STR(ins.argument_label, "somewhere"));
}


TEST(read_instruction_recognizes_return) {
	NInstruction ins = N_INSTRUCTION_INITIALIZER;
	NError error = N_ERROR_INITIALIZER;

	NLexer* lexer = WITH_INPUT("return 41303");
	ni_read_instruction(lexer, &ins, &error);

	ASSERT(ERROR_OK(&error));
	ASSERT(EQ_UINT(ins.opcode, N_OP_RETURN));
	ASSERT(EQ_INT(ins.arg_a, 41303));
}


static NLexer*
WITH_INPUT(const char* input) {
	END_INPUT();
	LEXER = ni_new_lexer(ni_new_istream_from_string(input));
	return LEXER;
}


static void
END_INPUT() {
	if (LEXER != NULL) {
		ni_destroy_lexer(LEXER);
		LEXER = NULL;
	}
}
