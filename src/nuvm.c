#include "nuvm.h"

#include "objects/primitives.h"
#include "objects/procedures.h"

void
n_init() {
	n_init_values();
	n_init_primitives();
	n_init_procedures();
}
