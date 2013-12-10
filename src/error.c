#include <string.h>
#include "util/types.h"

#include "error.h"

void n_clear_error(NError* error) {
	n_set_error(error, N_E_OK);
}

void n_set_error(NError* error, uint32_t code) {
	if (error != NULL) error->code = code;
}

void n_set_error_msg(NError* error, const char* msg) {
	if (error != NULL) {
		strncpy(error->message, msg, N_MAX_ERROR_MSG_LENGTH -1);
		error->message[N_MAX_ERROR_MSG_LENGTH-1] = '\0';
	}
}
