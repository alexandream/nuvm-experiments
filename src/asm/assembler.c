
#include "assembler.h"
#include "reader.h"

#include "../common/polyfills/p-strcasecmp.h"
#include "../common/polyfills/p-strdup.h"

/* Instantiating the resizable-array template */
#define ARRAY_TYPE_NAME NStringArray
#define ARRAY_CONTENTS_TYPE char*
#define ARRAY_PREFIX nsarray
#define ARRAY_ELEMENTS__SKIP
#include "../common/utils/resizable-array/full.h"

static uint32_t error_code_not_found,
                error_code_parsing_failed,
                error_constant_parsing_failed,
                error_constants_not_found,
                error_empty_constants_list,
                error_entry_point_out_of_bounds;

static void
consume_code_segment(NAssembler* self, NLexer* lexer, NError* error);

static void
consume_constant_list(NAssembler* self, NLexer* lexer, NError* error);

static void
consume_header_data(NAssembler* self, NLexer* lexer, NError* error);

static bool
is_constant_description_token(NTokenType);

/*
struct NStreamWriter {
	NStreamWriter_vtable* vtable;
};


struct NStreamWriter_vtable {
	void (*write_bytes)(NStreamWriter* self,
	                    uint8_t* data,
	                    uint32_t size,
	                    NError* error);

	void (*close)(NStreamWriter* self, NError* error);
}


typedef struct NConstantDescriptor NConstantDescriptor;


struct NConstantDescriptor {
	uint8_t type;
	uint64_t integer;
	double real;
	char* text;
}
*/

struct NAssembler {
	uint8_t version[3];
	uint16_t entry_point;
	uint16_t globals_count;


	NStringArray label_pool;

};


NAssembler*
ni_new_assembler() {
	NAssembler* result = (NAssembler*) malloc(sizeof(NAssembler));
	if (result == NULL) {
		/* FIXME: Add proper error handling for allocation errors. */
		return NULL;
	}
	/* FIXME: This array initializer has no way of signaling errors
	 * to the caller */
	nsarray_init(&result->label_pool, 64);
	return result;
}


void
ni_destroy_assembler(NAssembler* self) {
	NStringArray* pool = &self->label_pool;
	int32_t nelements = nsarray_count(pool);
	int32_t i;
	for (i = 0; i < nelements; i++) {
		char* elem = nsarray_get(pool, i);
		free(elem);
	}
	nsarray_destroy(pool);
	free(self);
}


void
ni_asm_set_version(NAssembler* self,
                   uint8_t major,
                   uint8_t minor,
                   uint8_t rev) {
	self->version[0] = major;
	self->version[1] = minor;
	self->version[2] = rev;
}


void
ni_asm_set_entry_point(NAssembler* self, uint16_t entry_point) {
	self->entry_point = entry_point;
}


void
ni_asm_set_globals_count(NAssembler* self, uint16_t globals_count) {
	self->globals_count = globals_count;
}


void
ni_asm_add_string_constant(NAssembler* self, const char* str) {

}


void
ni_asm_add_double_constant(NAssembler* self, double number) {

}


void
ni_asm_add_character_constant(NAssembler* self, const char* utf8_char) {

}


void
ni_asm_add_int32_constant(NAssembler* self, int32_t integer) {

}


void
ni_asm_add_procedure_constant(NAssembler* self,
                              uint16_t label_id,
                              uint16_t nlocals) {

}

uint16_t
ni_asm_get_label(NAssembler* self, const char* label, NError* error) {
	NStringArray* pool = &self->label_pool;
	int32_t size = nsarray_count(pool);
	char* new_element;
	int32_t i;
	/* Find the label in the pool, if it exists. */
	for (i = 0; i < size; i++) {
		char* elem = nsarray_get(pool, i);
		if (strcasecmp(label, elem) == 0) {
			/* FIXME: Should add a proper limiting factor to the for to make
			 * sure type constraints are preserved in the cast below */
			return (uint16_t) i;
		}
	}
	/* If we reach this, then the label is not yet present in the pool.
	 * We should add it. */
	/* FIXME: What do we do when this returns NULL? */
	new_element = strdup(label);
	return (uint16_t) nsarray_append(pool, new_element);
}


void
ni_asm_read_from_lexer(NAssembler* self, NLexer* lexer, NError* error) {

	consume_header_data(self, lexer, error);
	if (!n_error_ok(error)) return;
	
	consume_constant_list(self, lexer, error);
	if (!n_error_ok(error)) return;

	consume_code_segment(self, lexer, error);
}

/* Destructor for nuvm.asm.assembler.ConstantParsingFailed */
static void
destroy_error_with_child(NError* error) {
	NError* original_error = (NError*) error->data;
	if (original_error != NULL) {
		n_error_destroy(original_error);
		/* FIXME (#4): This should change if we change the cloning */
		free(original_error);
	}
	error->data = NULL;
}


void ni_init_assembler() {
	error_entry_point_out_of_bounds =
		n_register_error_type("nuvm.asm.assembler.EntryPointOutOfBounds",
		                      NULL,
		                      NULL);
	assert(error_entry_point_out_of_bounds < N_ERROR_LAST_VALID_ERROR);

	error_constants_not_found =
		n_register_error_type("nuvm.asm.assembler.ConstantsNotFound",
		                      NULL,
		                      NULL);
	assert(error_constants_not_found < N_ERROR_LAST_VALID_ERROR);

	error_empty_constants_list =
		n_register_error_type("nuvm.asm.assembler.EmptyConstantsList",
		                      NULL,
		                      NULL);
	assert(error_empty_constants_list < N_ERROR_LAST_VALID_ERROR);

	error_constant_parsing_failed =
		n_register_error_type("nuvm.asm.assembler.ConstantParsingFailed",
		                      NULL,
		                      destroy_error_with_child);
	assert(error_constant_parsing_failed < N_ERROR_LAST_VALID_ERROR);

	error_code_parsing_failed =
		n_register_error_type("nuvm.asm.assembler.CodeParsingFailed",
		                      NULL,
		                      destroy_error_with_child);
	assert(error_code_parsing_failed < N_ERROR_LAST_VALID_ERROR);
}


static void
consume_code_element(NAssembler* self,
                     NLexer* lexer,
                     NTokenType token,
                     NError* error) {
}


static void
consume_code_segment(NAssembler* self, NLexer* lexer, NError* error) {
	NTokenType next_token = ni_lexer_peek(lexer);
	if (next_token != NI_TK_KW_CODE) {
		error->type = error_code_not_found;
		return;
	}
	
	next_token = ni_lexer_peek(lexer);
	while (next_token != NI_TK_EOF) {
		consume_code_element(self, lexer, next_token, error);
		if (!n_error_ok(error)) {
			NError* original_error = n_error_clone(error);
			error->type = error_code_parsing_failed;
			error->data = (void*) original_error;
			return;
		}
		next_token = ni_lexer_peek(lexer);
	}
}


static void
consume_constant(NAssembler* self,
                 NLexer* lexer,
                 NTokenType token,
                 NError* error) {
	char* string_value;
	char* label_name;
	uint16_t num_locals, label_id;
	double double_value;
	int32_t int32_value;
	switch (token) {
		case NI_TK_KW_CHARACTER:
			ni_lexer_advance(lexer);
			ni_read_character_constant(lexer, &string_value, error);
			if (!n_error_ok(error)) return;
			ni_asm_add_character_constant(self, string_value);
			break;
		case NI_TK_KW_DOUBLE:
			ni_lexer_advance(lexer);
			ni_read_double_constant(lexer, &double_value, error);
			if (!n_error_ok(error)) return;
			ni_asm_add_double_constant(self, double_value);
			break;
		case NI_TK_KW_INT32:
			ni_lexer_advance(lexer);
			ni_read_int32_constant(lexer, &int32_value, error);
			if (!n_error_ok(error)) return;
			ni_asm_add_int32_constant(self, int32_value);
			break;
		case NI_TK_KW_PROCEDURE:
			ni_lexer_advance(lexer);
			ni_read_procedure_constant(lexer, &label_name, &num_locals, error);
			if (!n_error_ok(error)) return;
			label_id = ni_asm_get_label(self, label_name, error);
			if (!n_error_ok(error)) return;

			ni_asm_add_procedure_constant(self, label_id, num_locals);
			break;
		case NI_TK_KW_STRING:
			ni_lexer_advance(lexer);
			ni_read_string_constant(lexer, &string_value, error);
			if (!n_error_ok(error)) return;
			ni_asm_add_string_constant(self, string_value);
			break;
		default:
			break;
	}
}

static void
consume_constant_list(NAssembler* self, NLexer* lexer, NError* error) {
	uint16_t num_constants_found = 0;
	NTokenType next_token = ni_lexer_peek(lexer);
	if (next_token != NI_TK_KW_CONSTANTS)  {
		error->type = error_constants_not_found;
		return;
	}

	ni_lexer_advance(lexer);

	next_token = ni_lexer_peek(lexer);
	while (is_constant_description_token(next_token)) {
		consume_constant(self, lexer, next_token, error);
		if (!n_error_ok(error)) {
			NError* original_error = n_error_clone(error);
			error->type = error_constant_parsing_failed;
			error->data = (void*) original_error;
			return;
		}
		num_constants_found++;
		next_token = ni_lexer_peek(lexer);
	}

	if (num_constants_found == 0) {
		error->type = error_empty_constants_list;
		return;
	}
}


static void
consume_header_data(NAssembler* self, NLexer* lexer, NError* error) {
	uint8_t major, minor, revision;
	uint16_t entry_point, globals_count;

	ni_read_version(lexer, &major, &minor, &revision, error);
	if (!n_error_ok(error)) return;

	ni_asm_set_version(self, major, minor, revision);

	ni_read_entry_point(lexer, &entry_point, error);
	if (!n_error_ok(error)) return;

	ni_asm_set_entry_point(self, entry_point);

	ni_read_globals_count(lexer, &globals_count, error);
	if (!n_error_ok(error)) return;
	if (globals_count <= entry_point) {
		/* FIXME: This error handling was rushed. */
		/* Check if we need to add more data for the error to be meaningful
		 * for a caller. */
		error->type = error_entry_point_out_of_bounds;
		error->data = NULL;
	}

	ni_asm_set_globals_count(self, globals_count);
}


static bool
is_constant_description_token(NTokenType token) {
	return true;
}
