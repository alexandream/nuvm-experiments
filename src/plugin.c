#include <string.h>

#include "plugin.h"
#include "version.h"
#include "value.h"


bool
n_plugin_init(uint8_t* version, void* context_storage, char* error_msg) {
	NContext* context;
	if (version[0] != n_version[0] ||
	    version[1] != n_version[1] ||
	    version[2] != n_version[2]) {
		strncpy(error_msg, "Invalid version supplied.",
		        N_PLUGIN_ERROR_BUFFER_SIZE);
		error_msg[N_PLUGIN_ERROR_BUFFER_SIZE-1] = '\0';

		return false;
	}

	context = (NContext*) context_storage;

	context->n_to_string = n_to_string;
	context->n_bundle_new = n_bundle_new;
	context->n_bundle_close = n_bundle_close;
	context->n_bundle_get = n_bundle_get;
	context->n_bundle_set = n_bundle_set;

	return true;
}
