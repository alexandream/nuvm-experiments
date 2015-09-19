#include <stdlib.h>
#include <string.h>

#include "../common/polyfills/p-strcasecmp.h"
#include "../common/polyfills/p-strdup.h"

#include "errors.h"
#include "label-manager.h"

#define UNDEFINED_LABEL UINT32_MAX

#define BASE_POOL_SIZE 64

/* Instantiating the resizable-array template for the label pool */
#define N_DS_ARRAY_TYPE_NAME NLabelArray
#define N_DS_ARRAY_CONTENTS_TYPE NLabel
#define N_DS_ARRAY_PREFIX nlarray
#define N_DS_ARRAY_P_SKIP_STRUCT
#include "../common/utils/resizable-array/full.h"

struct NLabelManager {
	NLabelArray pool;
};


NLabelManager*
ni_new_label_manager(NError* error) {
	NError inner_error = N_ERROR_INITIALIZER;
	NLabelManager* result;
	n_error_reset(error);

	result = (NLabelManager*) malloc(sizeof(NLabelManager));
	if (result == NULL) {
		n_error_set(error, ni_a_errors.BadAllocation, NULL);
		return NULL;
	}

	ni_construct_label_manager(result, &inner_error);
	if (!n_error_ok(&inner_error)) {
		if (error != NULL) { *error = inner_error; }
		free(result);
		return NULL;
	}

	return result;
}


NLabelManager*
ni_construct_label_manager(NLabelManager* self, NError* error) {
	n_error_reset(error);
	nlarray_init(&self->pool, BASE_POOL_SIZE);
	return self;
}


void
ni_destruct_label_manager(NLabelManager* self) {
	int32_t nelements = nlarray_count(&self->pool);
	int32_t i;
	for (i = 0; i < nelements; i++) {
		NLabel elem = nlarray_get(&self->pool, i);
		free(elem.name);
	}
	nlarray_destroy(&self->pool);
}


void
ni_destroy_label_manager(NLabelManager* self) {
	ni_destruct_label_manager(self);
	free(self);
}


uint16_t
ni_label_manager_get(NLabelManager* self, const char* label, NError* error) {
	NLabelArray* pool = &(self->pool);
	int32_t size = nlarray_count(pool);
	NLabel new_element;
	int32_t i;
	/* Find the label in the pool, if it exists. */
	for (i = 0; i < size; i++) {
		NLabel elem = nlarray_get(pool, i);
		if (strcasecmp(label, elem.name) == 0) {
			/* FIXME: Should add a proper limiting factor to the for to make
			 * sure type constraints are preserved in the cast below */
			return (uint16_t) i;
		}
	}
	/* If we reach this, then the label is not yet present in the pool.
	 * We should add it. */
	/* FIXME: What do we do when this returns NULL? */
	new_element.name = strdup(label);
	new_element.definition = UNDEFINED_LABEL;

	return (uint16_t) nlarray_append(pool, new_element);
}


void
ni_label_manager_define(NLabelManager* self,
                        const char* label_name,
                        uint32_t definition,
                        NError* error) {
	NLabel* label;
	uint16_t label_id;

	n_error_reset(error);

	label_id = ni_label_manager_get(self, label_name, error);
	if (!n_error_ok(error)) return;

	label = &nlarray_elements(&self->pool)[label_id];
	label->definition = definition;
}


uint32_t
ni_label_manager_get_definition(NLabelManager* self,
                                const char* label_name,
                                NError* error) {
	NLabel* label;
	uint16_t label_id;

	n_error_reset(error);

	label_id = ni_label_manager_get(self, label_name, error);
	if (!n_error_ok(error)) return 0;

	label = &nlarray_elements(&self->pool)[label_id];
	return label->definition;
}
