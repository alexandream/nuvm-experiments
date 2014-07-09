#include <stdlib.h>
#include <string.h>

#include "errors.h"
#include "polyfills/strdup.h"

#define MAX_ERROR_TYPES 1024

typedef struct {
	const char* name;
	error_print_func_t print_func;
	error_destroy_func_t destroy_func;
} NErrorType;



/* TODO: Make this error type pool dynamic. */
static uint32_t next_error_type_id = 0;
static NErrorType error_type_pool[MAX_ERROR_TYPES];


uint32_t
n_find_error_type(const char* name) {
	uint32_t i;
	/* TODO: Make this a non linear search. */
	for (i = 0; i < next_error_type_id; i++) {
		if (strcmp(name, error_type_pool[i].name) == 0) {
			return i;
		}
	}
	return N_ERROR_TYPE_NOT_FOUND;
}


uint32_t
n_register_error_type(const char* name,
                      error_print_func_t print,
                      error_destroy_func_t destroy) {
	NErrorType* error_type;
	uint32_t type_id;
	if (name == NULL || *name == '\0') {
		return N_ERROR_TYPE_NOT_FOUND;
	}

	/* TODO: This range check must change when the type pool is dynamic. */
	if (n_find_error_type(name) != N_ERROR_TYPE_NOT_FOUND) {
		return N_ERROR_TYPE_ALREADY_REGISTERED;
	}
	else if (next_error_type_id >= MAX_ERROR_TYPES) {
		return N_ERROR_TYPE_OVERFLOW;
	}
	
	type_id = next_error_type_id++;
	error_type = &error_type_pool[type_id];

	error_type->name = name;
	error_type->print_func = print;
	error_type->destroy_func = destroy;
	return type_id;
}


const char*
n_error_print(NError* error) {
	NErrorType* error_type = &error_type_pool[error->type];
	if (error_type->print_func) {
		return error_type->print_func(error);
	}
	else {
		return strdup(error_type->name);
	}
}


void
n_error_destroy(NError* error) {
	NErrorType* error_type = &error_type_pool[error->type];
	if (error_type->destroy_func) {
		error_type->destroy_func(error);
	}
}

bool
n_error_ok(NError* error) {
	return error->type == 0;
}
