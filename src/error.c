#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include "util/types.h"

#include "error.h"

void n_error_clear(NError* error) {
	n_error_set(error, N_E_OK);
}

bool n_error_ok(NError* error) {
	return (error == NULL || error->code == N_E_OK);
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

/* FIXME: This doesn't check the size of the buffer on error msg
 *        and could potentially trigger a nasty buffer overflow. */
void n_error_set_msgf(NError* error, const char* format, ...) {
	if (error != NULL) {
		va_list ap;
		va_start(ap, format);
		vsprintf(error->message, format, ap);
		va_end(ap);
	}
}
