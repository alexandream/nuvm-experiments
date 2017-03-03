#ifndef NHG_C_ERRORS
#define NHG_C_ERRORS

#include "polyfills/p-stdint.h"

#define N_ERROR_TYPE_NOT_FOUND          UINT32_MAX
#define N_ERROR_TYPE_OVERFLOW           UINT32_MAX - 1
#define N_ERROR_TYPE_ALREADY_REGISTERED UINT32_MAX - 2
#define N_ERROR_LAST_VALID_ERROR        UINT32_MAX - 3

typedef struct {
	uint32_t type;
	void* data;
} NError;

/* FIXME (#4): This initializer fails if any GC is involved on error data */
#define N_ERROR_INITIALIZER { 0, NULL }

typedef char* (*error_print_func_t)(NError*);
typedef void (*error_destroy_func_t)(NError*);


uint32_t
n_register_error_type(const char* name,
                      error_print_func_t print,
                      error_destroy_func_t destroy);

uint32_t
n_find_error_type(const char* name);

char*
n_error_print(NError* error);

char*
n_error_print_type_name(NError* error);

void
n_error_destroy(NError* error);

void
n_error_reset(NError* error);

NError*
n_error_clone(NError* error);

void
n_error_copy(NError* dest, NError* src);

bool
n_error_ok(NError* error);

void
n_error_set(NError* error, uint32_t type, void* data);

void
n_error_destroy_by_freeing(NError* error);

void
n_init_errors();
#endif
