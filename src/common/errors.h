#ifndef __N__COMMON__ERRORS__H__
#define __N__COMMON__ERRORS__H__

#include "std-types.h"

#define N_ERROR_TYPE_NOT_FOUND          UINT32_MAX
#define N_ERROR_TYPE_OVERFLOW           UINT32_MAX - 1
#define N_ERROR_TYPE_ALREADY_REGISTERED UINT32_MAX - 2
#define N_ERROR_LAST_VALID_ERROR        UINT32_MAX - 3

typedef struct {
	uint32_t type;
	void* data;
} NError;

#define N_ERROR_INITIALIZER { 0, NULL }

typedef const char* (*error_print_func_t)(NError*);
typedef void (*error_destroy_func_t)(NError*);


uint32_t
n_register_error_type(const char* name,
                      error_print_func_t print,
                      error_destroy_func_t destroy);

uint32_t
n_find_error_type(const char* name);

const char*
n_error_print(NError* error);

const char*
n_error_print_type_name(NError* error);

void
n_error_destroy(NError* error);

void
n_error_reset(NError* error);

bool
n_error_ok(NError* error);

void
n_init_errors();
#endif
