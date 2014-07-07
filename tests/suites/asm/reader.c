#define SUITE_NAME Reader 

#include "../../test-suite.h"
#include "common/errors.h"

#include "reader.h"
#include "lexer.h"
#include "streams.h"
void
ni_read_version(ni_lexer_t* lexer,
                uint8_t* major,
                uint8_t* minor,
                uint8_t* revision,
                n_error_t* error) {
}

void
ni_read_entry_point(ni_lexer_t* lexer,
                    uint16_t* entry_point,
                    n_error_t* error) {
}

void
ni_read_globals_count(ni_lexer_t* lexer,
                      uint16_t* num_globals,
                      n_error_t* error) {

}

uint8_t*
n_asm_build_file(const char* fname) {
	ni_stream_t* stream = ni_new_stream_from_path(fname);
	ni_lexer_t* lexer = ni_new_lexer(stream);
	
	uint8_t major, minor, revision;
	uint16_t num_globals, entry_point;

	ni_token_type_t next_token;

	n_error_t error = { 0, };
	
	ni_read_version(lexer, &major, &minor, &revision, &error);
	if (!n_error_ok(&error)) goto handle_error;
	/* TODO: Set version on the builder. */

	ni_read_entry_point(lexer, &entry_point, &error);
	if (!n_error_ok(&error)) goto handle_error;
	/* TODO: Set entry point on the builder. */

	ni_read_globals_count(lexer, &num_globals, &error);
	if (!n_error_ok(&error)) goto handle_error;
	/* TODO: Set globals count on the builder. */

	next_token = ni_lexer_peek(lexer);
	if (next_token != NI_TK_KW_CONSTANTS) {
		/* TODO: Create error, expected '.constants', found something else. */
	}
	{

		char* const_string_val;
		double const_double_val;
		char* const_proc_label_val;
		uint16_t const_proc_num_locals_val;
		int32_t const_int32_val;

		next_token = ni_lexer_peek(lexer);
		while (next_token != NI_TK_KW_CODE) {

			switch(next_token) {
				case NI_TK_KW_STRING:

					break;
				case NI_TK_KW_DOUBLE:
					break;
				case NI_TK_KW_CHARACTER:
					break;
				case NI_TK_KW_PROCEDURE:
					break;
				case NI_TK_KW_INT32:
					break;
				default:
					/* TODO: Create error: expected constant descriptor keyword,
					 * got token %s. */
					break;
			}
			if (!n_error_ok(&error)) goto handle_error;
			next_token = ni_lexer_peek(lexer);
		}
	}
	if (next_token != NI_TK_KW_CODE) {
		/* TODO: Create error, expected '.code', found something else. */
	}

	while (next_token != NI_TK_EOF) {
		switch(next_token) {
			case NI_TK_OP_RETURN:
				break;
			case NI_TK_OP_MOVE:
				break;
			case NI_TK_OP_GLOBAL_REF:
				break;
			case NI_TK_OP_GLOBAL_SET:
				break;
			case NI_TK_OP_JUMP:
				break;
			case NI_TK_OP_JUMP_IF:
				break;
			case NI_TK_OP_EQ: /* fall-through */
			case NI_TK_OP_LT: /* fall-through */
			case NI_TK_OP_LE: /* fall-through */
			case NI_TK_OP_GT: /* fall-through */
			case NI_TK_OP_GE: /* fall-through */
				break;
			case NI_TK_OP_OR:  /* fall-through */
			case NI_TK_OP_AND: /* fall-through */
			case NI_TK_OP_ADD: /* fall-through */
			case NI_TK_OP_SUB: /* fall-through */
			case NI_TK_OP_MUL: /* fall-through */
			case NI_TK_OP_DIV: /* fall-through */
				break;
			case NI_TK_OP_NOT:
				break;
			case NI_TK_OP_LOAD_BOOL:
				break;
			default:
				/* TODO: Create error: expected opcode, got token %s. */
				break;
		}
		if (!n_error_ok(&error)) goto handle_error;
		next_token = ni_lexer_peek(lexer);
	}


handle_error:
	return NULL;
}

