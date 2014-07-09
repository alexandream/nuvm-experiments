#define SUITE_NAME Reader 

#include "../../test-suite.h"
#include "common/errors.h"

#include "reader.h"
#include "lexer.h"
#include "streams.h"

uint8_t*
n_asm_build_file(const char* fname) {
	NStream* stream = ni_new_stream_from_path(fname);
	NLexer* lexer = ni_new_lexer(stream);
	
	uint8_t major, minor, revision;
	uint16_t num_globals, entry_point;

	NTokenType next_token;

	NError error = { 0, };
	
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

		double const_double_val;
		char* const_string_val;
		char* const_proc_label_val;
		uint16_t const_proc_num_locals_val;
		int32_t const_int32_val;

		next_token = ni_lexer_peek(lexer);
		while (next_token != NI_TK_KW_CODE) {
			const_string_val = NULL;
			const_proc_label_val = NULL;

			switch(next_token) {
				case NI_TK_KW_STRING:
					ni_read_string_constant(lexer, &const_string_val, &error);
					if (!n_error_ok(&error)) goto handle_error;
					/* TODO: Do something with the string. */
					free(const_string_val);
					break;
				case NI_TK_KW_DOUBLE:
					ni_read_double_constant(lexer, &const_double_val, &error);
					if (!n_error_ok(&error)) goto handle_error;
					/* TODO: Do something with the double. */
					break;
				case NI_TK_KW_CHARACTER:
					ni_read_character_constant(lexer,
					                           &const_string_val,
					                           &error);
					if (!n_error_ok(&error)) goto handle_error;
					/* TODO: Do something with the character. */
					free(const_string_val);
					break;
				case NI_TK_KW_PROCEDURE:
					ni_read_procedure_constant(lexer,
					                           &const_proc_label_val,
					                           &const_proc_num_locals_val,
					                           &error);
					if (!n_error_ok(&error)) goto handle_error;
					/* TODO: Do something with the procedure data. */
					free(const_proc_label_val);
					break;
				case NI_TK_KW_INT32:
					ni_read_int32_constant(lexer, &const_int32_val, &error);
					if (!n_error_ok(&error)) goto handle_error;
					/* TODO: Do something with the int32. */
					break;
				default:
					/* TODO: Create error: expected constant descriptor keyword,
					 * got token %s. */
					goto handle_error;
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
				goto handle_error;
				break;
		}
		if (!n_error_ok(&error)) goto handle_error;
		next_token = ni_lexer_peek(lexer);
	}


handle_error:
	/* TODO: Actually handle error and free resources. */
	return NULL;
}

