#define AT_SUITE_NAME Tokenizer
#include "../../test-suite.h"

static n_word
_get_word_from_string(const char* input);


TEST(get_word_ignores_spaces) {
	n_word word = _get_word_from_string("  \n\t\t  ");
	ASSERT(word.length == 0);
	n_destroy_word(word);
}

TEST(get_word_accepts_identifiers) {
	ASSERT(0);
}



static n_word
_get_word_from_string(const char* input) {
	n_stream* stream = n_new_string_stream(input);
	n_word word = n_get_next_word(stream);
	n_destroy_stream(stream);
	return word;
}
