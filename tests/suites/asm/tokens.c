#define SUITE_NAME Tokens
#include <string.h>

#include "../../test-suite.h"

#include "streams.h"
#include "tokens.h"

static NStream* STREAM = NULL;

static void
WITH_STREAM(const char* str);

static NCheckResult
CK_EOF() {
	char buffer[256+1];
	NTokenType token = ni_get_next_token(STREAM, buffer, 256);
	NI_MAKE_CHECK(token == NI_TK_EOF,
		MF("Expected EOF, got token type %s with lexeme %s.",
		   ni_get_token_name(token), buffer), { /* empty finalizer */ });
}

static NCheckResult
CK_TOKEN(NTokenType expected_type, const char* expected_lexeme) {
	char buffer[256+1];
	NTokenType type = ni_get_next_token(STREAM, buffer, 256);
	bool condition =
		type == expected_type && strcmp(expected_lexeme, buffer) == 0;
	NI_MAKE_CHECK(condition,
		MF("Expected token type %s with lexeme %s. "
		   "Got token type %s with lexeme %s",
		   ni_get_token_name(expected_type), expected_lexeme,
		   ni_get_token_name(type), buffer),
		{ /* empty finalizer */ });
}
#define ASSERT_EOF() ASSERT_CHECK(CK_EOF())


#define ASSERT_TOKEN(type, lexeme) ASSERT_CHECK(CK_TOKEN(type, lexeme))



TEST(ignores_only_spaces) {
	WITH_STREAM("  \n \t \n \n \t  ");
	ASSERT_EOF();
}


TEST(reads_single_character_identifier) {
	WITH_STREAM("h");
	ASSERT_TOKEN(NI_TK_UNRECOGNIZED_OPCODE, "h");
	ASSERT_EOF();
}


TEST(reads_word_identifier) {
	WITH_STREAM("hello");
	ASSERT_TOKEN(NI_TK_UNRECOGNIZED_OPCODE, "hello");
	ASSERT_EOF();
}


TEST(reads_case_sensitive_identifier) {
	WITH_STREAM("hElLo");
	ASSERT_TOKEN(NI_TK_UNRECOGNIZED_OPCODE, "hElLo");
	ASSERT_EOF();
}


TEST(reads_identifier_with_digits) {
	WITH_STREAM("h1234");
	ASSERT_TOKEN(NI_TK_UNRECOGNIZED_OPCODE, "h1234");
	ASSERT_EOF();
}


TEST(reads_identifier_with_hyphen) {
	WITH_STREAM("hello-world");
	ASSERT_TOKEN(NI_TK_UNRECOGNIZED_OPCODE, "hello-world");
	ASSERT_EOF();
}


TEST(reads_identifier_with_starting_local_register_lead) {
	WITH_STREAM("Lambda");
	ASSERT_TOKEN(NI_TK_UNRECOGNIZED_OPCODE, "Lambda");
	ASSERT_EOF();
}


TEST(reads_identifier_with_starting_global_register_lead) {
	WITH_STREAM("Gambling");
	ASSERT_TOKEN(NI_TK_UNRECOGNIZED_OPCODE, "Gambling");
	ASSERT_EOF();
}


TEST(reads_identifier_with_starting_constant_register_lead) {
	WITH_STREAM("Command");
	ASSERT_TOKEN(NI_TK_UNRECOGNIZED_OPCODE, "Command");
	ASSERT_EOF();
}


TEST(ignores_spaces) {
	WITH_STREAM(" \n\t h1234 \t\n ");
	ASSERT_TOKEN(NI_TK_UNRECOGNIZED_OPCODE, "h1234");
	ASSERT_EOF();
}


TEST(reads_many_identifiers) {
	WITH_STREAM("  hello world-o \t g00   ");
	ASSERT_TOKEN(NI_TK_UNRECOGNIZED_OPCODE, "hello");
	ASSERT_TOKEN(NI_TK_UNRECOGNIZED_OPCODE, "world-o");
	ASSERT_TOKEN(NI_TK_UNRECOGNIZED_OPCODE, "g00");
	ASSERT_EOF();
}


TEST(reads_zero_decimal_integer) {
	WITH_STREAM("  0 ");
	ASSERT_TOKEN(NI_TK_DEC_INTEGER, "0");
	ASSERT_EOF();
}


TEST(reads_decimal_integer_with_leading_zero) {
	WITH_STREAM(" 01234 ");
	ASSERT_TOKEN(NI_TK_DEC_INTEGER, "01234");
	ASSERT_EOF();
}


TEST(reads_decimal_integer) {
	WITH_STREAM(" 10588 ");
	ASSERT_TOKEN(NI_TK_DEC_INTEGER, "10588");
	ASSERT_EOF();
}


TEST(reads_negative_decimal_integer) {
	WITH_STREAM("-123456789");
	ASSERT_TOKEN(NI_TK_DEC_INTEGER, "-123456789");
	ASSERT_EOF();
}


TEST(reads_single_digit_decimal_integer) {
	WITH_STREAM("8");
	ASSERT_TOKEN(NI_TK_DEC_INTEGER, "8");
	ASSERT_EOF();
}


TEST(reads_hexadecimal_integer) {
	WITH_STREAM("0x12345678");
	ASSERT_TOKEN(NI_TK_HEX_INTEGER, "0x12345678");
	ASSERT_EOF();
}


TEST(reads_hexadecimal_integer_high_digits) {
	WITH_STREAM("0xdeadbeef");
	ASSERT_TOKEN(NI_TK_HEX_INTEGER, "0xdeadbeef");
	ASSERT_EOF();
}


TEST(reads_hexadecimal_integer_case_sensitive_high_digits) {
	WITH_STREAM("0xDeadBeef");
	ASSERT_TOKEN(NI_TK_HEX_INTEGER, "0xDeadBeef");
	ASSERT_EOF();
}

TEST(reads_hexadecimal_single_digit) {
	WITH_STREAM("0xA");
	ASSERT_TOKEN(NI_TK_HEX_INTEGER, "0xA");
	ASSERT_EOF();
}


TEST(reads_empty_string) {
	WITH_STREAM("\"\"");
	ASSERT_TOKEN(NI_TK_STRING, "\"\"");
	ASSERT_EOF();
}


TEST(reads_string) {
	WITH_STREAM("\"hello\"");
	ASSERT_TOKEN(NI_TK_STRING, "\"hello\"");
	ASSERT_EOF();
}


TEST(reads_string_with_spaces) {
	WITH_STREAM("\"  hello, world   \"");
	ASSERT_TOKEN(NI_TK_STRING, "\"  hello, world   \"");
	ASSERT_EOF();
}


TEST(reads_string_with_inner_quotes) {
	WITH_STREAM("\"  hello, \\\"world  \"");
	ASSERT_TOKEN(NI_TK_STRING, "\"  hello, \\\"world  \"");
	ASSERT_EOF();
}


TEST(reads_label_def) {
	WITH_STREAM("my-label:");
	ASSERT_TOKEN(NI_TK_LABEL_DEF, "my-label:");
	ASSERT_EOF();
}


TEST(reads_local_register_reference_zero) {
	WITH_STREAM("L:0");
	ASSERT_TOKEN(NI_TK_REGISTER, "L:0");
	ASSERT_EOF();
}


TEST(reads_global_register_reference_zero) {
	WITH_STREAM("G:0");
	ASSERT_TOKEN(NI_TK_REGISTER, "G:0");
	ASSERT_EOF();
}


TEST(reads_constant_register_reference_zero) {
	WITH_STREAM("C:0");
	ASSERT_TOKEN(NI_TK_REGISTER, "C:0");
	ASSERT_EOF();
}


TEST(reads_local_register_reference) {
	WITH_STREAM("L:1234");
	ASSERT_TOKEN(NI_TK_REGISTER, "L:1234");
	ASSERT_EOF();
}


TEST(reads_global_register_reference) {
	WITH_STREAM("G:4321");
	ASSERT_TOKEN(NI_TK_REGISTER, "G:4321");
	ASSERT_EOF();
}


TEST(reads_constant_register_reference) {
	WITH_STREAM("C:987");
	ASSERT_TOKEN(NI_TK_REGISTER, "C:987");
	ASSERT_EOF();
}


TEST(reads_keyword_character) {
	WITH_STREAM(".character");
	ASSERT_TOKEN(NI_TK_KW_CHARACTER, ".character");
	ASSERT_EOF();
}

TEST(reads_keyword_code) {
	WITH_STREAM(".code");
	ASSERT_TOKEN(NI_TK_KW_CODE, ".code");
	ASSERT_EOF();
}


TEST(reads_keyword_constants) {
	WITH_STREAM(".constants");
	ASSERT_TOKEN(NI_TK_KW_CONSTANTS, ".constants");
	ASSERT_EOF();
}


TEST(reads_keyword_double) {
	WITH_STREAM(".double");
	ASSERT_TOKEN(NI_TK_KW_DOUBLE, ".double");
	ASSERT_EOF();
}


TEST(reads_keyword_entry_point) {
	WITH_STREAM(".entry-point");
	ASSERT_TOKEN(NI_TK_KW_ENTRY_POINT, ".entry-point");
	ASSERT_EOF();
}


TEST(reads_keyword_globals_count) {
	WITH_STREAM(".globals-count");
	ASSERT_TOKEN(NI_TK_KW_GLOBALS_COUNT, ".globals-count");
	ASSERT_EOF();
}


TEST(reads_keyword_int32) {
	WITH_STREAM(".int32");
	ASSERT_TOKEN(NI_TK_KW_INT32, ".int32");
	ASSERT_EOF();
}


TEST(reads_keyword_procedure) {
	WITH_STREAM(".procedure");
	ASSERT_TOKEN(NI_TK_KW_PROCEDURE, ".procedure");
	ASSERT_EOF();
}


TEST(reads_keyword_string) {
	WITH_STREAM(".string");
	ASSERT_TOKEN(NI_TK_KW_STRING, ".string");
	ASSERT_EOF();
}


TEST(reads_keyword_version) {
	WITH_STREAM(".version");
	ASSERT_TOKEN(NI_TK_KW_VERSION, ".version");
	ASSERT_EOF();
}


TEST(reads_unrecognized_keyword) {
	WITH_STREAM(".whatever-we-want-2");
	ASSERT_TOKEN(NI_TK_UNRECOGNIZED_KW, ".whatever-we-want-2");
	ASSERT_EOF();
}


TEST(reads_real_zero) {
	WITH_STREAM("0.0");
	ASSERT_TOKEN(NI_TK_REAL, "0.0");
	ASSERT_EOF();
}


TEST(reads_real_positive_number) {
	WITH_STREAM("3.1415927");
	ASSERT_TOKEN(NI_TK_REAL, "3.1415927");
	ASSERT_EOF();
}


TEST(reads_real_negative_number) {
	WITH_STREAM("-10.588");
	ASSERT_TOKEN(NI_TK_REAL, "-10.588");
	ASSERT_EOF();
}


TEST(reads_label_reference) {
	WITH_STREAM("@the-label");
	ASSERT_TOKEN(NI_TK_LABEL, "@the-label");
	ASSERT_EOF();
}


TEST(reads_op_move) {
	WITH_STREAM("move");
	ASSERT_TOKEN(NI_TK_OP_MOVE, "move");
	ASSERT_EOF();
}


TEST(reads_op_global_ref) {
	WITH_STREAM("global-ref");
	ASSERT_TOKEN(NI_TK_OP_GLOBAL_REF, "global-ref");
	ASSERT_EOF();
}


TEST(reads_op_global_set) {
	WITH_STREAM("global-set");
	ASSERT_TOKEN(NI_TK_OP_GLOBAL_SET, "global-set");
	ASSERT_EOF();
}


TEST(reads_op_jump) {
	WITH_STREAM("jump");
	ASSERT_TOKEN(NI_TK_OP_JUMP, "jump");
	ASSERT_EOF();
}


TEST(reads_op_jump_if) {
	WITH_STREAM("jump-if");
	ASSERT_TOKEN(NI_TK_OP_JUMP_IF, "jump-if");
	ASSERT_EOF();
}


TEST(reads_op_eq) {
	WITH_STREAM("eq");
	ASSERT_TOKEN(NI_TK_OP_EQ, "eq");
	ASSERT_EOF();
}


TEST(reads_op_lt) {
	WITH_STREAM("lt");
	ASSERT_TOKEN(NI_TK_OP_LT, "lt");
	ASSERT_EOF();
}


TEST(reads_op_le) {
	WITH_STREAM("le");
	ASSERT_TOKEN(NI_TK_OP_LE, "le");
	ASSERT_EOF();
}


TEST(reads_op_gt) {
	WITH_STREAM("gt");
	ASSERT_TOKEN(NI_TK_OP_GT, "gt");
	ASSERT_EOF();
}


TEST(reads_op_ge) {
	WITH_STREAM("ge");
	ASSERT_TOKEN(NI_TK_OP_GE, "ge");
	ASSERT_EOF();
}


TEST(reads_op_not) {
	WITH_STREAM("not");
	ASSERT_TOKEN(NI_TK_OP_NOT, "not");
	ASSERT_EOF();
}


TEST(reads_op_or) {
	WITH_STREAM("or");
	ASSERT_TOKEN(NI_TK_OP_OR, "or");
	ASSERT_EOF();
}


TEST(reads_op_and) {
	WITH_STREAM("and");
	ASSERT_TOKEN(NI_TK_OP_AND, "and");
	ASSERT_EOF();
}


TEST(reads_op_add) {
	WITH_STREAM("add");
	ASSERT_TOKEN(NI_TK_OP_ADD, "add");
	ASSERT_EOF();
}


TEST(reads_op_sub) {
	WITH_STREAM("sub");
	ASSERT_TOKEN(NI_TK_OP_SUB, "sub");
	ASSERT_EOF();
}


TEST(reads_op_mul) {
	WITH_STREAM("mul");
	ASSERT_TOKEN(NI_TK_OP_MUL, "mul");
	ASSERT_EOF();
}


TEST(reads_op_div) {
	WITH_STREAM("div");
	ASSERT_TOKEN(NI_TK_OP_DIV, "div");
	ASSERT_EOF();
}


TEST(reads_op_load_bool) {
	WITH_STREAM("load-bool");
	ASSERT_TOKEN(NI_TK_OP_LOAD_BOOL, "load-bool");
	ASSERT_EOF();
}


static void
END_STREAM() {
	if (STREAM) {
		ni_destroy_stream(STREAM);
		STREAM = NULL;
	}
}


static void
WITH_STREAM(const char* str) {
	END_STREAM();
	STREAM = ni_new_stream_from_string(str);
}
