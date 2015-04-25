#include <string.h>
#include <stdlib.h>

#include "assembler.h"
#include "reader.h"

#include "../common/polyfills/p-strcasecmp.h"
#include "../common/polyfills/p-strdup.h"

#define UNDEFINED_LABEL UINT32_MAX

typedef struct {
	char* name;
	uint32_t definition;
} NLabel;

typedef struct {
	uint8_t type;
	int64_t integer;
	uint16_t aux_integer;
	double real;
	char* text;
} NConstantDescriptor;

#define NI_CONSTANT_INITIALIZER { 0xFF, 0, 0, 0.0, NULL }


/* Instantiating the resizable-array template for the label pool */
#define ARRAY_TYPE_NAME NLabelArray
#define ARRAY_CONTENTS_TYPE NLabel
#define ARRAY_PREFIX nlarray
#define ARRAY_ELEMENTS__SKIP
#include "../common/utils/resizable-array/full.h"

/* Instantiating the resizable-array template for the instructions pool */
#define ARRAY_TYPE_NAME NCodePool
#define ARRAY_CONTENTS_TYPE NInstruction*
#define ARRAY_PREFIX ncpool
#define ARRAY_ELEMENTS__SKIP
#include "../common/utils/resizable-array/full.h"

/* Instantiating the resizable-array template for the constants pool */
#define ARRAY_TYPE_NAME NConstantPool
#define ARRAY_CONTENTS_TYPE NConstantDescriptor
#define ARRAY_PREFIX ncopool
#define ARRAY_ELEMENTS__SKIP
#include "../common/utils/resizable-array/full.h"

static uint32_t error_code_not_found,
                error_code_parsing_failed,
                error_constant_parsing_failed,
                error_constants_not_found,
                error_empty_constants_list,
                error_entry_point_out_of_bounds,
                error_unexpected_token;
static void
consume_code_segment(NAssembler* self, NLexer* lexer, NError* error);

static void
consume_constant_list(NAssembler* self, NLexer* lexer, NError* error);

static void
consume_header_data(NAssembler* self, NLexer* lexer, NError* error);


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

*/

struct NAssembler {
	uint8_t version[3];
	uint16_t entry_point;
	uint16_t globals_count;

	NCodePool code_pool;
	NLabelArray label_pool;
	NConstantPool constant_pool;
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
	nlarray_init(&result->label_pool, 64);
	ncpool_init(&result->code_pool, 1024);
	ncopool_init(&result->constant_pool, 128);
	return result;
}


void
ni_destroy_assembler(NAssembler* self) {
	NLabelArray* pool = &self->label_pool;
	int32_t nelements = nlarray_count(pool);
	int32_t i;
	for (i = 0; i < nelements; i++) {
		NLabel elem = nlarray_get(pool, i);
		free(elem.name);
	}

	nelements = ncopool_count(&self->constant_pool);
	for (i = 0; i < nelements; i++) {
		NConstantDescriptor descriptor = ncopool_get(&self->constant_pool, i);
		if (descriptor.text != NULL) {
			free(descriptor.text);
		}
	}

	nelements = ncpool_count(&self->code_pool);
	for (i = 0; i < nelements; i++) {
		NInstruction* instruction = ncpool_get(&self->code_pool, i);
		free(instruction);
	}

	nlarray_destroy(pool);
	ncpool_destroy(&self->code_pool);
	ncopool_destroy(&self->constant_pool);
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
ni_asm_add_instruction(NAssembler* self,
                       NInstruction* instruction,
                       NError* error) {
	NInstruction* owned_instruction =
		ni_asm_instruction_clone(instruction, error);
	if (!n_error_ok(error)) return;
	if (instruction->argument_label != NULL) {
		owned_instruction->argument_label_id =
			ni_asm_get_label(self, instruction->argument_label, error);
		if (!n_error_ok(error)) goto handle_error;
	}

	ncpool_append(&self->code_pool, owned_instruction);

	return;
handle_error:
	if (owned_instruction != NULL) {
		ni_asm_instruction_destroy(owned_instruction);
		free(owned_instruction);
	}
}



void
ni_asm_add_string_constant(NAssembler* self, char* str) {
	NConstantDescriptor descriptor = NI_CONSTANT_INITIALIZER;
	descriptor.type = NI_CONSTANT_STRING;
	descriptor.text = str;
	ncopool_append(&self->constant_pool, descriptor);
}


void
ni_asm_add_double_constant(NAssembler* self, double number) {
	NConstantDescriptor descriptor = NI_CONSTANT_INITIALIZER;
	descriptor.type = NI_CONSTANT_DOUBLE;
	descriptor.real = number;
	ncopool_append(&self->constant_pool, descriptor);
}


void
ni_asm_add_character_constant(NAssembler* self, char* utf8_char) {
	NConstantDescriptor descriptor = NI_CONSTANT_INITIALIZER;
	descriptor.type = NI_CONSTANT_CHARACTER;
	descriptor.text = utf8_char;
	ncopool_append(&self->constant_pool, descriptor);
}


void
ni_asm_add_int32_constant(NAssembler* self, int32_t integer) {
	NConstantDescriptor descriptor = NI_CONSTANT_INITIALIZER;
	descriptor.type = NI_CONSTANT_INT32;
	descriptor.integer = integer;
	ncopool_append(&self->constant_pool, descriptor);
}


void
ni_asm_add_procedure_constant(NAssembler* self,
                              uint16_t label_id,
                              uint16_t nlocals) {
	NConstantDescriptor descriptor = NI_CONSTANT_INITIALIZER;
	descriptor.type = NI_CONSTANT_PROCEDURE;
	descriptor.integer = label_id;
	descriptor.aux_integer = nlocals;
	ncopool_append(&self->constant_pool, descriptor);
}

uint16_t
ni_asm_get_label(NAssembler* self, const char* label, NError* error) {
	NLabelArray* pool = &self->label_pool;
	int32_t size = nlarray_count(pool);
	NLabel new_element;
	int32_t i;
	/* Find the label in the pool, if it exists. */
	for (i = 0; i < size; i++) {
		NLabel elem = nlarray_get(pool, i);
		if (strcasecmp(label, elem.name) == 0) {
			/* FIXME: Should add a proper limiting factor to the for to make
			 * sure type constraints are preserved in the cast below */
			return (uint16_t) i;
		}
	}
	/* If we reach this, then the label is not yet present in the pool.
	 * We should add it. */
	/* FIXME: What do we do when this returns NULL? */
	new_element.name = strdup(label);
	new_element.definition = UNDEFINED_LABEL;

	return (uint16_t) nlarray_append(pool, new_element);
}


void
ni_asm_define_label(NAssembler* self, const char* label_name, NError* error) {
	NLabel label;

	uint16_t label_id = ni_asm_get_label(self, label_name, error);
	if (!n_error_ok(error)) return;

	label = nlarray_get(&self->label_pool, label_id);
	label.definition = ncpool_count(&self->code_pool);
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

	error_unexpected_token =
		n_register_error_type("nuvm.asm.assembler.UnexpectedToken",
		                      NULL,
		                      n_error_destroy_by_freeing);
	assert(error_unexpected_token < N_ERROR_LAST_VALID_ERROR);
}


static void
consume_code_element(NAssembler* self,
                     NLexer* lexer,
                     NTokenType token,
                     NError* error) {
	if(token == NI_TK_LABEL_DEF) {
		NToken label_def = ni_lexer_read(lexer);
		size_t label_len = strlen(label_def.lexeme);
		char* label_name = label_def.lexeme;
		label_name[label_len] = '\0';
		ni_asm_define_label(self, label_name, error);
		ni_destroy_token(label_def);
	}
	else if (ni_token_is_opcode(token)) {
		NInstruction instruction = N_INSTRUCTION_INITIALIZER;
		ni_read_instruction(lexer, &instruction, error);
		if (!n_error_ok(error)) return;

		ni_asm_add_instruction(self, &instruction, error);
		ni_asm_instruction_destroy(&instruction);
		if (!n_error_ok(error)) return;
	}
	else {
		NToken token_data = ni_lexer_copy(lexer);
		error->type = error_unexpected_token;
		error->data = (void*) ni_token_lift(token_data);
		return;
	}

}


static void
consume_code_segment(NAssembler* self, NLexer* lexer, NError* error) {
	NTokenType next_token = ni_lexer_peek(lexer);
	if (next_token != NI_TK_KW_CODE) {
		error->type = error_code_not_found;
		return;
	}

	ni_lexer_advance(lexer);
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
	NToken token_data;
	char* string_value;
	char* label_name;
	uint16_t num_locals, label_id;
	double double_value;
	int32_t int32_value;
	switch (token) {
		case NI_TK_KW_CHARACTER:
			ni_read_character_constant(lexer, &string_value, error);
			if (!n_error_ok(error)) return;
			ni_asm_add_character_constant(self, string_value);
			break;
		case NI_TK_KW_DOUBLE:
			ni_read_double_constant(lexer, &double_value, error);
			if (!n_error_ok(error)) return;
			ni_asm_add_double_constant(self, double_value);
			break;
		case NI_TK_KW_INT32:
			ni_read_int32_constant(lexer, &int32_value, error);
			if (!n_error_ok(error)) return;
			ni_asm_add_int32_constant(self, int32_value);
			break;
		case NI_TK_KW_PROCEDURE:
			ni_read_procedure_constant(lexer, &label_name, &num_locals, error);
			if (!n_error_ok(error)) return;
			label_id = ni_asm_get_label(self, label_name, error);
			if (!n_error_ok(error)) return;

			ni_asm_add_procedure_constant(self, label_id, num_locals);
			break;
		case NI_TK_KW_STRING:
			ni_read_string_constant(lexer, &string_value, error);
			if (!n_error_ok(error)) return;
			ni_asm_add_string_constant(self, string_value);
			break;
		default:
			token_data = ni_lexer_copy(lexer);
			error->type = error_unexpected_token;
			error->data = (void*) ni_token_lift(token_data);
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
	while (ni_token_is_constant_keyword(next_token)) {
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
	if (num_constants_found <= self->entry_point) {
		/* FIXME: This error handling was rushed. */
		/* Check if we need to add more data for the error to be meaningful
		 * for a caller. */
		error->type = error_entry_point_out_of_bounds;
		error->data = NULL;
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

	ni_asm_set_globals_count(self, globals_count);
}
