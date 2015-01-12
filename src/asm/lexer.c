#include <stdlib.h>
#include "common/polyfills/p-strdup.h"

#include "lexer.h"
#define LEXEME_BUFFER_SIZE 256

static char discard_buffer[LEXEME_BUFFER_SIZE+1];

struct NLexer {
	NStream* input;
	NTokenType cache;
	bool has_cache;
	char buffer[LEXEME_BUFFER_SIZE+1];
};


void
ni_destroy_lexer(NLexer* self) {
	ni_destroy_stream(self->input);
}

NLexer*
ni_new_lexer(NStream* input) {
	NLexer* self = (NLexer*) malloc(sizeof(NLexer));
	if (self == NULL) {
		return NULL;
	}
	self->input = input;
	self->has_cache = false;
	self->cache = NI_TK_UNKNOWN;
	self->buffer[LEXEME_BUFFER_SIZE] = '\0';
	return self;
}


void
ni_lexer_advance(NLexer* self) {
	if (!self->has_cache) {
		ni_get_next_token(self->input, discard_buffer, LEXEME_BUFFER_SIZE);
	}
	else {
		self->has_cache = false;
	}
}


NTokenType
ni_lexer_peek(NLexer* self) {
	if (!self->has_cache) {
		self->cache =
			ni_get_next_token(self->input, self->buffer, LEXEME_BUFFER_SIZE);
		self->has_cache = true;
	}
	return self->cache;
}


NToken
ni_lexer_read(NLexer* self) {
	NToken result = ni_lexer_copy(self);
	ni_lexer_advance(self);
	return result;
}


NToken
ni_lexer_copy(NLexer* self) {
	NToken result;
	result.type = ni_lexer_peek(self);
	result.lexeme = strdup(self->buffer);

	return result;
}
