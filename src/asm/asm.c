#include "common/common.h"
#include "errors.h"
#include "label-manager.h"

void
n_init_asm() {
	char* error_msg;
	uint8_t error_code;
	n_init_common();

	if (!n_init_asm_errors(&error_code, &error_msg)) {
		/* TODO: Do something with initialization errors here. */
	}
}
