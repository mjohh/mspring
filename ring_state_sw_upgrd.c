#include "ring_def.h"
#include "ring_state.h"
#include "ring_state_switch.h"
#include "ring_state_wtr.h"
#include "ring_state_primary.h"
#include "ring_aps_controller.h"
#include "ring_funcs.h"
#include "ring_state_sf.h"
#include "ring_state_sd.h"
#include "ring_rule_s.h"
#include "ring_state.h"
//// here impl switch upgrade,degrade and occupy.

// get brq caused switch
// if ret side is NUM_SIDES means coexist short brq and local dq.coulde be:
//  1)local brq-local brq
//  2)short brq-short brq
void get_reason_brq_and_short_side(struct aps_controller *aps, enum brq_code *brq, enum side *side)
{
    if (is_coexist_local_brq(aps)) {
        // for coexist brq, choose WEST brq as switch reason.
        *brq = get_local_brq(aps, WEST);
        *side = NUM_SIDES;
    } else if (is_coexist_local_brq_and_short_brq(aps, WEST)) {
        *brq = get_local_brq(aps, WEST);
        *side = WEST;
    } else if (is_coexist_local_brq_and_short_brq(aps, EAST)) {
        *brq = get_local_brq(aps, EAST);
        *side = EAST;
    } else if (is_only_local_brq(aps, WEST)) {
        *brq = get_local_brq(aps, WEST);
        *side = WEST;
    } else if (is_only_local_brq(aps, EAST)) {
        *brq = get_local_brq(aps, EAST);
        *side = EAST;
    } else if (is_coexist_short_brq(aps)) {
        // for coexist brq, choose WEST brq as switch reason.
        *brq = get_brq_for_me(aps, WEST);
        *side = NUM_SIDES;
    } else if (is_only_short_brq(aps, WEST)) {
        *brq = get_brq_for_me(aps, WEST);
        *side = WEST;
    } else if (is_only_short_brq(aps, EAST)) {
        *brq = get_brq_for_me(aps, EAST);
        *side = EAST;
    } else {
        *brq = NR;
        *side = aps->short_side;	//keep short side unchanged
    }
}


void exit_cur_sw_state(struct aps_controller *aps)
{
    end_state_exit(aps);
    sw_state_exit(aps);
}

void drop_switch_if_occupy(struct aps_controller *aps)
{
    enum brq_code new_brq;
    enum side new_side;
    enum side cur_side;
    
    get_reason_brq_and_short_side(aps, &new_brq, &new_side);
    cur_side = aps->short_side;
    aps->short_side = new_side;
    
    if (SW_STARTUP == aps->cur_sw_state) {
        // at switch init state, need do nothing.
        return;
    }
    if (new_side == cur_side) {
        ///use cur_end_state directly, do nothing
    } else			// occupy
    {
        // drop cur switch state firstly, and new state will start from begining.
        exit_cur_sw_state(aps);
    }
}
