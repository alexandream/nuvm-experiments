#ifndef __N__ASM__LEXER__H__
#define __N__ASM__LEXER__H__

#include "istreams.h"
#include "tokens.h"

typedef struct NLexer NLexer;

void
ni_destroy_lexer(NLexer* self);

NLexer*
ni_new_lexer(NIStream* input);

void
ni_lexer_advance(NLexer* self);

NToken
ni_lexer_copy(NLexer* self);

NTokenType
ni_lexer_peek(NLexer* self);

NToken
ni_lexer_read(NLexer* self);

#endif
