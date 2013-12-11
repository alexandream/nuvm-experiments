
#include "type-info.h"

void 
n_type_init(NType* self, const char* name) {
	self->id = 0;
	self->name = name;
}
