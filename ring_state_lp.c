#include "ring_def.h"
#include "ring_state.h"
#include "ring_state_switch.h"
#include "ring_state_wtr.h"
#include "ring_state_primary.h"
#include "ring_aps_controller.h"
#include "ring_funcs.h"
#include "ring_state_sd.h"
#include "ring_state_sf.h"
#include "ring_rule_s.h"
#include "ring_state_sw_upgrd.h"
#include "ring_state.h"

#include "ring_dbg.h"

#include <assert.h>
void lp_state_exit(struct aps_controller* aps)
{
    aps->cur_end_state = END_START_UP;
}

void lp_state_run(struct aps_controller* aps)
{
    if (is_isolated_node(aps)) {
        to_start_up(aps);
	} else if (is_tail_end(aps)) {
		assert(is_only_local_brq(aps, WEST) || is_only_local_brq(aps, EAST));
        update_for_only_local_brq(aps, aps->short_side, IDLE_STATUS);
		update_node_state(aps, IDLE);
	    end_state_set(aps, END_START_UP);
	} else {// head end
	    assert(is_only_short_brq(aps, WEST) || is_only_short_brq(aps, EAST));
        update_for_only_short_brq(aps, aps->short_side, IDLE_STATUS, IDLE_ACK_STATUS);
		update_node_state(aps, IDLE);
	    end_state_set(aps, END_START_UP);
	}
	//ring_print(aps->node_id, "   exit lp, state = %s\n", end_state_name(aps->cur_end_state));
}