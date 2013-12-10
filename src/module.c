#include <stdlib.h>

#include "module.h"

struct NModule {
	uint16_t register_count;
	uint16_t entry;
};

NModule*
n_new_module(uint16_t register_count, uint16_t entry, NError* error) {
	n_clear_error(error);
	if (register_count == 0) {
		n_set_error(error, N_E_INVALID_ARGUMENT);
		n_set_error_msg(error, "register_count");
	}

	return NULL;
}
	

void
n_destroy_module(NModule* self) {

}
