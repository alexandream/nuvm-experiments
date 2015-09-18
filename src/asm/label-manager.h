#ifndef NHG_A_LABEL_MANAGER
#define NHG_A_LABEL_MANAGER

#include "../common/errors.h"
#include "../common/polyfills/p-stdint.h"

typedef struct NLabelManager NLabelManager;

NLabelManager*
ni_new_label_manager(NError* error);

void
ni_destroy_label_manager(NLabelManager* self);


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


void
ni_init_label_manager();
#endif /* NHG_A_LABEL_MANAGER */
