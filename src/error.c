#include <string.h>
#include "util/types.h"

#include "error.h"

void n_error_clear(NError* error) {
	n_error_set(error, N_E_OK);
}

void n_error_set(NError* error, uint32_t code) {
	if (error != NULL) {
		error->code = code;
		error->message[0] = '\0';
	}
}

void n_error_set_msg(NError* error, const char* msg) {
	if (error != NULL) {
		strncpy(error->message, msg, N_MAX_ERROR_MSG_LENGTH -1);
		error->message[N_MAX_ERROR_MSG_LENGTH-1] = '\0';
	}
}
