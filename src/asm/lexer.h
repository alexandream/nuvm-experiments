#ifndef __N__ASM__LEXERS__H__
#define __N__ASM__LEXERS__H__

#include "streams.h"
#include "tokens.h"

typedef struct ni_lexer_t ni_lexer_t;

void
ni_destroy_lexer(ni_lexer_t* self);

ni_lexer_t*
ni_new_lexer(ni_stream_t* input);

ni_token_t
ni_lexer_peek(ni_lexer_t* self);

void
ni_lexer_advance(ni_lexer_t* self);

#endif
