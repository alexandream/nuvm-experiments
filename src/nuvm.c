#include "nuvm.h"
#include "symbol-pool.h"

#include "objects/bundles.h"
#include "objects/primitives.h"
#include "objects/procedures.h"
#include "objects/strings.h"


void
n_init() {
	n_init_values();

	n_init_bundles();
	n_init_primitives();
	n_init_procedures();
	n_init_strings();
}
