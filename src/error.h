#ifndef __NUVM__ERROR_H__
#define __NUVM__ERROR_H__

#include "util/types.h"

enum {
	N_E_OK = 0,
	N_E_BAD_ALLOCATION,
	N_E_INVALID_ARGUMENT,
	N_E_OUT_OF_BOUNDS,
	N_E_TYPE_REGISTRY_CONFLICT
};

#define N_MAX_ERROR_MSG_LENGTH 128

typedef struct {
	uint32_t code;
	char message[N_MAX_ERROR_MSG_LENGTH];
} NError;

void n_error_clear(NError* error);

void n_error_set(NError* error, uint32_t code);

void n_error_set_msg(NError* error, const char* msg);

#endif

