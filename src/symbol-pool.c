#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util/strings.h"

#include "memory.h"
#include "type-info.h"

#include "symbol-pool.h"


/* FIXME: Using an array as the pool, but a better mapping data structure
 * should be used. */
#define ARRAY_TYPE_NAME     NSymbolArray
#define ARRAY_PREFIX        n_symbol_array
#define ARRAY_CONTENTS_TYPE const char*
#define ARRAY_DESTROY__SKIP
#include "util/resizable_array.h"

#ifndef SYMBOL_POOL_INITIAL_SIZE
#define SYMBOL_POOL_INITIAL_SIZE 1024
#endif

static NSymbolArray _pool;

int32_t
_find_symbol(const char* contents);


void
n_symbol_pool_init() {
	n_symbol_array_init(&_pool, SYMBOL_POOL_INITIAL_SIZE);
	if (n_symbol_array_elements(&_pool) == NULL) {
		fprintf(stderr, "Unable to initialize storage for symbols. "
		                "Aborting.\n");
		exit(1);
	}
}


const char*
n_symbol_pool_fetch(int32_t id) {
	return n_symbol_array_get(&_pool, id);
}


NValue
n_symbol_pool_get_symbol(const char* contents, NError* error) {
	n_error_clear(error);
	if (contents != NULL) {
		int32_t index = _find_symbol(contents);
		if (index < 0) {
			const char* dup_contents = duplicate_string(contents);
			if (dup_contents == NULL) {
				n_error_set(error, N_E_BAD_ALLOCATION);
				n_error_set_msg(error, "contents");
				return N_UNDEFINED;
			}
			index = n_symbol_array_append(&_pool, dup_contents);
			if (index < 0) {
				n_error_set(error, N_E_BAD_ALLOCATION);
				n_error_set_msg(error, "inner-pool");
				return N_UNDEFINED;
			}
		}
		return n_wrap_symbol(index);
	}
	else {
		n_error_set(error, N_E_INVALID_ARGUMENT);
		n_error_set_msg(error, "contents");
		return N_UNDEFINED;
	}
}


int32_t
_find_symbol(const char* contents) {
	int32_t i;
	int32_t count = n_symbol_array_count(&_pool);
	for (i = 0; i < count; i++) {
		if (strcmp(contents, n_symbol_array_get(&_pool, i)) == 0) {
			return i;
		}
	}
	return -1;
}
