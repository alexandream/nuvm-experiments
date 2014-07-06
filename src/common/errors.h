#ifndef __N__COMMON__ERRORS__H__
#define __N__COMMON__ERRORS__H__

#include "std-types.h"

#define N_ERROR_TYPE_NOT_FOUND          UINT32_MAX
#define N_ERROR_TYPE_OVERFLOW           UINT32_MAX - 1
#define N_ERROR_TYPE_ALREADY_REGISTERED UINT32_MAX - 2

typedef struct {
	uint32_t type;
	void* data;
} n_error_t;

typedef const char* (*error_print_func_t)(n_error_t*);
typedef void (*error_destroy_func_t)(n_error_t*);


uint32_t
n_register_error_type(const char* name,
                      error_print_func_t print,
                      error_destroy_func_t destroy);

uint32_t
n_find_error_type(const char* name);

const char*
n_error_print(n_error_t* error);

void
n_error_destroy(n_error_t* error);


#endif
