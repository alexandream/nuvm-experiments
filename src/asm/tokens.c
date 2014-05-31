#include "tokens.h"


void
n_destroy_token(n_token_t token) {

}

n_token_t
n_get_next_token(n_stream_t* stream) {
	n_token_t result;
	result.type = N_TK_EOF;
	result.lexeme = "";
	return result;
}
