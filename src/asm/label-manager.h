#ifndef NHG_A_LABEL_MANAGER
#define NHG_A_LABEL_MANAGER

#include "../common/errors.h"
#include "../common/polyfills/p-stdint.h"

typedef struct {
	char* name;
	uint32_t definition;
} NLabel;


#define N_DS_ARRAY_TYPE_NAME NLabelArray
#define N_DS_ARRAY_CONTENTS_TYPE NLabel
#define N_DS_ARRAY_P_SKIP_PROTOTYPES
#include "../common/utils/resizable-array/header.h"

typedef struct {
	NLabelArray pool;
} NLabelManager;

NLabelManager*
ni_new_label_manager(NError* error);

void
ni_destroy_label_manager(NLabelManager* self);

NLabelManager*
ni_construct_label_manager(NLabelManager* self, NError* error);

void
ni_destruct_label_manager(NLabelManager* self);

uint16_t
ni_label_manager_get(NLabelManager* self,
                     const char* label,
                     NError* error);


void
ni_label_manager_define(NLabelManager* self,
                        const char* label,
                        uint32_t definition,
                        NError* error);

uint32_t
ni_label_manager_get_definition(NLabelManager* self,
                                const char* label,
                                NError* error);

uint32_t
ni_label_manager_get_definition_by_id(NLabelManager* self,
                                      uint16_t label_id,
                                      NError* error);

#endif /* NHG_A_LABEL_MANAGER */
