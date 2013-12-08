#include "ring_def.h"
#include "ring_state.h"
#include "ring_state_switch.h"
#include "ring_state_wtr.h"
#include "ring_state_primary.h"
#include "ring_aps_controller.h"
#include "ring_rule_s.h"
#include "ring_funcs.h"
#include "ring_state_sf.h"
#include "ring_dbg.h"

#include <assert.h>

void sf_start_up_change(struct aps_controller *aps)
{
    start_up_change(aps);
}

void sf_idle_and_waiting_change(struct aps_controller *aps)
{
    if (is_isolated_node(aps)) {
        to_start_up(aps);
    } else if (is_tail_end(aps)) {
        enum side long_side = OTHER_SIDE(aps->short_side);
        assert(is_only_local_brq(aps, WEST) || is_only_local_brq(aps, EAST));
        
        if (is_valid_long_reply_for_brq(aps, get_local_brq(aps, aps->short_side), long_side)) {
            tail_to_brsw(aps);
        } else {
            tail_to_idle_and_waiting(aps);
        }
    } else {			// head end
        enum side long_side = OTHER_SIDE(aps->short_side);
        assert(is_only_short_brq(aps, WEST) || is_only_short_brq(aps, EAST));
        
        if (is_valid_long_reply_for_brq(aps, get_short_brq(aps, aps->short_side), long_side)) {
            head_to_brsw0(aps);
        } else {
            head_to_idle_and_waiting(aps);
        }
    }
}

void sf_brsw_change(struct aps_controller *aps)
{
    brsw_change(aps);
}

void sf_state_exit(struct aps_controller *aps)
{
    aps->cur_end_state = END_START_UP;
}

void sf_state_run(struct aps_controller *aps)
{
    assert(aps);
    switch (aps->cur_end_state) {
        case END_START_UP:
            sf_start_up_change(aps);
            break;
        case END_IDLE_AND_WAITING:
            sf_idle_and_waiting_change(aps);
            break;
        case END_BRSW:
            sf_brsw_change(aps);
            break;
        default:
            sf_brsw_change(aps);	//if success from sd br state, upgrade to sf brsw
            break;
    }
    //ring_print(aps->node_id, "   exit sf, state = %sn", end_state_name(aps->cur_end_state));
}
