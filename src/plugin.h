#ifndef __NUVM__PLUGIN_H__
#define __NUVM__PLUGIN_H__

#include "error.h"
#include "objects/bundles.h"

#define N_PLUGIN_ERROR_BUFFER_SIZE 1024

typedef struct {
	NValue (*n_to_string)(NValue);

	NBundle* (*n_bundle_new)(uint16_t, NError*);
	void (*n_bundle_close)(NBundle*);
	NValue (*n_bundle_get)(NBundle*, NValue, NError*);
	void (*n_bundle_set)(NBundle*, NValue, NValue, NError*);
} NContext;


bool
n_plugin_init(uint8_t* version, void* context_storage, char*);

#endif
