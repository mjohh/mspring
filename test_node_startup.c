#include "ring_def.h"
#include "ring_state.h"
#include "ring_state_switch.h"
#include "ring_state_wtr.h"
#include "ring_state_primary.h"
#include "ring_aps_controller.h"
#include "ring_funcs.h"
#include "unit_test_if.h"

void tx_default_kbytes_before_ne_ready(struct aps_controller * aps) {
	aps->is_ne_ready = 0;
	prim_state_run(aps);
	assert_tx_kbytes(aps, WEST, 0, 0, 0, 0, 0);
	assert_tx_kbytes(aps, EAST, 0, 0, 0, 0, 0);
}