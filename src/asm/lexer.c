#include <stdlib.h>
#include "common/polyfills/strdup.h"

#include "lexer.h"
#define LEXEME_BUFFER_SIZE 256

static char discard_buffer[LEXEME_BUFFER_SIZE+1];

struct ni_lexer_t {
	ni_stream_t* input;
	ni_token_type_t cache;
	bool has_cache;
	char buffer[LEXEME_BUFFER_SIZE+1];
};


void
ni_destroy_lexer(ni_lexer_t* self) {
	ni_destroy_stream(self->input);
}

ni_lexer_t*
ni_new_lexer(ni_stream_t* input) {
	ni_lexer_t* self = (ni_lexer_t*) malloc(sizeof(ni_lexer_t));
	if (self == NULL) {
		return NULL;
	}
	self->input = input;
	self->has_cache = false;
	self->cache = NI_TK_UNKNOWN;
	self->buffer[LEXEME_BUFFER_SIZE] = '\0';
	return self;
}

ni_token_t
ni_lexer_peek(ni_lexer_t* self) {
	ni_token_t result;
	if (!self->has_cache) {
		self->cache =
			ni_get_next_token(self->input, self->buffer, LEXEME_BUFFER_SIZE);
		self->has_cache = true;
	}
	result.type = self->cache;
	result.lexeme = strdup(self->buffer);

	return result;
}

void
ni_lexer_advance(ni_lexer_t* self) {
	if (!self->has_cache) {
		ni_get_next_token(self->input, discard_buffer, LEXEME_BUFFER_SIZE);
	}
	else {
		self->has_cache = false;
	}
}

