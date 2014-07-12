#define SUITE_NAME Reader 

#include "../../test-suite.h"
#include "common/errors.h"

#include "reader.h"
#include "lexer.h"
#include "streams.h"

static NLexer* LEXER = NULL;

static NLexer*
WITH_INPUT(const char* input);


TEARDOWN {
	if (LEXER != NULL) {
		ni_destroy_lexer(LEXER);
		LEXER = NULL;
	}
}


TEST(read_version_recognizes_input) {
	uint8_t major, minor, revision;
	NError error = N_ERROR_INITIALIZER;
	NLexer* lexer = WITH_INPUT(".version 5 200 44");
	ni_read_version(lexer, &major, &minor, &revision, &error);
	ASSERT_EOK(&error);
	ASSERT_EQ_I64(major, 5);
	ASSERT_EQ_I64(minor, 200);
	ASSERT_EQ_I64(revision, 44);
}


static NLexer*
WITH_INPUT(const char* input) {
	LEXER = ni_new_lexer(ni_new_stream_from_string(input));
	return LEXER;
}
