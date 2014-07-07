#define SUITE_NAME Lexer
#include <string.h>

#include "../../test-suite.h"

#include "lexer.h"

static ni_lexer_t* LEXER = NULL;

static void
ADVANCE();

static void
WITH_INPUT(const char* str);


#define ASSERT_PEEK(_type) do{\
	ni_token_type_t type = _type;\
	ni_token_type_t token = ni_lexer_peek(LEXER);\
	ASSERT_MSG(token == type,\
		MF("Expected token type %d, got token type %d.",\
		   type, token));\
} while(0)


TEARDOWN {
	if (LEXER != NULL) {
		ni_destroy_lexer(LEXER);
		LEXER = NULL;
	}
}


TEST(peek_reads_first_token) {
	WITH_INPUT("hello");
	ASSERT_PEEK(NI_TK_UNRECOGNIZED_OPCODE);
}


TEST(peek_reads_eof_on_empty) {
	WITH_INPUT("");
	ASSERT_PEEK(NI_TK_EOF);
}


TEST(double_peek_returns_first_token_twice) {
	WITH_INPUT("hello 3.14");
	ASSERT_PEEK(NI_TK_UNRECOGNIZED_OPCODE);
	ASSERT_PEEK(NI_TK_UNRECOGNIZED_OPCODE);
}


TEST(peek_advance_peek_reads_first_two_tokens) {
	WITH_INPUT("hello 3.14");
	ASSERT_PEEK(NI_TK_UNRECOGNIZED_OPCODE);
	ADVANCE();
	ASSERT_PEEK(NI_TK_REAL);
}


TEST(advance_peek_reads_second_token) {
	WITH_INPUT("hello 3.14");
	ADVANCE();
	ASSERT_PEEK(NI_TK_REAL);
}


TEST(peek_reads_eof_after_advancing_to_end) {
	WITH_INPUT("hello");
	ADVANCE();
	ASSERT_PEEK(NI_TK_EOF);
}


TEST(peek_reads_eof_after_advancing_past_end) {
	WITH_INPUT("hello");
	ADVANCE();
	ADVANCE();
	ASSERT_PEEK(NI_TK_EOF);
}



static void
ADVANCE() {
	ni_lexer_advance(LEXER);
}


static void
WITH_INPUT(const char* str) {
	LEXER = ni_new_lexer(ni_new_stream_from_string(str));
}
