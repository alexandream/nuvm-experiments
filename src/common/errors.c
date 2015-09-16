#include <stdlib.h>
#include <string.h>

#include "errors.h"
#include "polyfills/p-strdup.h"

#define MAX_ERROR_TYPES 1024

typedef struct {
	const char* name;
	error_print_func_t print_func;
	error_destroy_func_t destroy_func;
} NErrorType;



/* TODO: Make this error type pool dynamic. */
static uint32_t next_error_type_id = 3;
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


char*
n_error_print(NError* error) {
	NErrorType* error_type = &error_type_pool[error->type];
	if (error_type->print_func) {
		return error_type->print_func(error);
	}
	else {
		return n_error_print_type_name(error);
	}
}

char*
n_error_print_type_name(NError* error) {
	NErrorType* error_type = &error_type_pool[error->type];
	return strdup(error_type->name);
}


NError*
n_error_clone(NError* error) {
	/* FIXME (#4): MUST use different allocation strategy here. */
	/* If we fail to do this, this will break when errors with garbage
	 * data in them get cloned. */
	NError* cloned = (NError*) malloc(sizeof(NError));
	if (cloned != NULL) {
		cloned->type = error->type;
		cloned->data = error->data;
	}
	return cloned;
}


void
n_error_destroy(NError* error) {
	NErrorType* error_type = &error_type_pool[error->type];
	if (error_type->destroy_func) {
		error_type->destroy_func(error);
	}
	error->data = NULL;
}


void
n_error_reset(NError* error) {
	if (error != NULL && error->type != 0) {
		n_error_destroy(error);
		error->type = 0;
	}
}


bool
n_error_ok(NError* error) {
	return error->type == 0;
}


void
n_error_destroy_by_freeing(NError* error) {
	void* data = error->data;
	if (data != NULL) {
		free(data);
	}
}


void
n_init_errors() {
	NErrorType* success = &error_type_pool[0];
	NErrorType* unknown = &error_type_pool[1];
	NErrorType* bad_alloc = &error_type_pool[2];

	success->name = "nuvm.Ok";
	success->print_func = NULL;
	success->destroy_func = NULL;

	unknown->name = "nuvm.UnknownError";
	unknown->print_func = NULL;
	unknown->destroy_func = NULL;

	bad_alloc->name = "nuvm.BadAllocation";
	bad_alloc->print_func = NULL;
	bad_alloc->destroy_func = NULL;
}
