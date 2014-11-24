
#include "assembler.h"

#include "../common/polyfills/p-strcasecmp.h"
#include "../common/polyfills/p-strdup.h"
 
/* Instantiating the resizable_array template */
#define ARRAY_TYPE_NAME NStringArray
#define ARRAY_CONTENTS_TYPE char*
#define ARRAY_PREFIX nsarray
#define ARRAY_ELEMENTS__SKIP
#include "../common/utils/resizable_array.h"

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
ni_asm_add_string_constant(NAssembler* self, const char* str);

void
ni_asm_add_double_constant(NAssembler* self, double number);

void
ni_asm_add_character_constant(NAssembler* self, const char* utf8_char);

void
ni_asm_add_int32_constant(NAssembler* self, int32_t integer);

void
ni_asm_add_procedure_constant(NAssembler* self,
                              uint16_t label_id,
                              uint16_t nlocals);

uint16_t
ni_asm_get_label(NAssembler* self, const char* label) {
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
