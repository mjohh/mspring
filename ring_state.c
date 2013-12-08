#include "ring_def.h"
#include "ring_state.h"
#include "ring_state_switch.h"
#include "ring_state_wtr.h"
#include "ring_state_primary.h"
#include "ring_aps_controller.h"
#include "ring_rule_s.h"
#include "ring_funcs.h"
#include "ring_state_sd.h"
#include "ring_dbg.h"
#include "ring_state_sf.h"
#include "ring_state_primary.h"

#include <assert.h>
/*
 [idle_state] [switch_state] [startup_state] [wtr_state] [pass_state] [kbyte_pass_state]
 |
 [sf_state] [sd_state] [fs_state] [ms_state] [lp_state] [exer_state]
 |
 [startup_state] [idle_and_waiting_state] [brsw_state]
 */

void end_state_init(struct aps_controller *aps)
{
    assert(aps);
    aps->cur_end_state = END_START_UP;
}

void end_state_fini(struct aps_controller *aps)
{
    // do nothing
}

void end_state_run(struct aps_controller *aps)
{
    // do nothing
}

void end_state_exit(struct aps_controller *aps)
{
    assert(aps);
    aps->cur_end_state = END_START_UP;
}

void end_state_set(struct aps_controller *aps, enum end_state_id next_state)
{
    assert(aps);
    aps->cur_end_state = next_state;
}

void to_brsw(struct aps_controller *aps)
{
    enum node_state state = (aps->short_side == EAST ? BRSW_W : BRSW_E);
    update_node_state(aps, state);
    end_state_set(aps, END_BRSW);
}

void to_idle_and_waiting(struct aps_controller *aps)
{
    update_node_state(aps, IDLE);
    end_state_set(aps, END_IDLE_AND_WAITING);
}

void to_br_and_waiting(struct aps_controller *aps)
{
    enum node_state state = (aps->short_side == EAST ? BR_W : BR_E);
    update_node_state(aps, state);
    end_state_set(aps, END_BR_AND_WAITING);
}

void to_start_up(struct aps_controller *aps)
{
    update_kbytes_for_isolated_node(aps);
    update_node_state(aps, IDLE);
    end_state_set(aps, END_START_UP);
}

void head_to_brsw0(struct aps_controller *aps)
{
    to_brsw(aps);
    update_for_only_short_brq(aps, aps->short_side, BRSW_STATUS, NOT_IDLE_ACK_STATUS);
}

void head_to_brsw(struct aps_controller *aps)
{
    to_brsw(aps);
    update_for_only_short_brq(aps, aps->short_side, BRSW_STATUS, BRSW_STATUS);
}

void tail_to_brsw(struct aps_controller *aps)
{
    to_brsw(aps);
    update_for_only_local_brq(aps, aps->short_side, BRSW_STATUS);
};

void head_to_idle_and_waiting(struct aps_controller *aps)
{
    to_idle_and_waiting(aps);
    update_for_only_short_brq(aps, aps->short_side, IDLE_STATUS, IDLE_ACK_STATUS);
}

void tail_to_idle_and_waiting(struct aps_controller *aps)
{
    to_idle_and_waiting(aps);
    update_for_only_local_brq(aps, aps->short_side, IDLE_STATUS);
}

void head_to_br_and_waiting(struct aps_controller *aps)
{
    to_br_and_waiting(aps);
    update_for_only_short_brq(aps, aps->short_side, BR_STATUS, NOT_IDLE_ACK_STATUS);
}

void tail_to_br_and_waiting(struct aps_controller *aps)
{
    to_br_and_waiting(aps);
    update_for_only_local_brq(aps, aps->short_side, BR_STATUS);
}

char *end_state_name(int state)
{
    switch (state) {
        case END_START_UP:
            return "END_START_UP";
        case END_IDLE_AND_WAITING:
            return "END_IDLE_AND_WAITING";
        case END_BR_AND_WAITING:
            return "END_BR_AND_WAITING";
        case END_BRSW:
            return "END_BRSW";
        default:
            return "err end state";
    }
}

void start_up_change(struct aps_controller *aps)
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
            head_to_brsw(aps);
        } else {
            head_to_idle_and_waiting(aps);
        }
    }
}

void idle_and_waiting_change(struct aps_controller *aps)
{
    if (is_isolated_node(aps)) {
        to_start_up(aps);
    } else if (is_tail_end(aps)) {
        enum side long_side = OTHER_SIDE(aps->short_side);
        assert(is_only_local_brq(aps, WEST) || is_only_local_brq(aps, EAST));
        
        if (is_valid_long_reply_for_brq(aps, get_local_brq(aps, aps->short_side), long_side)) {
            tail_to_br_and_waiting(aps);
        } else {
            tail_to_idle_and_waiting(aps);
        }
    } else {			// head end
        enum side long_side = OTHER_SIDE(aps->short_side);
        assert(is_only_short_brq(aps, WEST) || is_only_short_brq(aps, EAST));
        
        if (is_valid_long_reply_for_brq(aps, get_short_brq(aps, aps->short_side), long_side)) {
            head_to_br_and_waiting(aps);
        } else {
            head_to_idle_and_waiting(aps);
        }
    }
}

void br_and_waiting_change(struct aps_controller *aps)
{
    if (is_isolated_node(aps)) {
        to_start_up(aps);
    } else if (is_tail_end(aps)) {
        enum side long_side = OTHER_SIDE(aps->short_side);
        assert(is_only_local_brq(aps, WEST) || is_only_local_brq(aps, EAST));
        
        if (is_valid_long_reply_for_brq(aps, get_local_brq(aps, aps->short_side), long_side)) {
            tail_to_brsw(aps);
        } else {
            tail_to_br_and_waiting(aps);
        }
    } else {			// head end
        enum side long_side = OTHER_SIDE(aps->short_side);
        assert(is_only_short_brq(aps, WEST) || is_only_short_brq(aps, EAST));
        
        if (is_valid_long_reply_for_brq(aps, get_short_brq(aps, aps->short_side), long_side)) {
            head_to_brsw0(aps);
        } else {
            head_to_br_and_waiting(aps);
        }
    }
}

void brsw_change(struct aps_controller *aps)
{
    if (is_isolated_node(aps)) {
        to_start_up(aps);
    } else if (is_tail_end(aps)) {
        enum side long_side = OTHER_SIDE(aps->short_side);
        assert(is_only_local_brq(aps, WEST) || is_only_local_brq(aps, EAST));
        
        if (is_valid_long_reply_for_brq(aps, get_local_brq(aps, aps->short_side), long_side)) {
            tail_to_brsw(aps);
        }
    } else {			// head end
        enum side long_side = OTHER_SIDE(aps->short_side);
        assert(is_only_short_brq(aps, WEST) || is_only_short_brq(aps, EAST));
        
        if (is_valid_long_reply_for_brq(aps, get_short_brq(aps, aps->short_side), long_side)) {
            head_to_brsw(aps);
        }
    }
}

int is_coexist_sd_ms_exer(struct aps_controller *aps)
{
    if (is_different_span_for_local_sd_ms_exer(aps, WEST))
        return 1;
    if (is_different_span_for_local_sd_ms_exer(aps, EAST))
        return 1;
    if (is_different_span_for_short_sd_ms_exer(aps, WEST))
        return 1;
    if (is_different_span_for_short_sd_ms_exer(aps, EAST))
        return 1;
    return 0;
}

void update_for_coexist_sd_ms_exer(struct aps_controller *aps)
{
    if (is_different_span_for_local_sd_ms_exer(aps, WEST)) {
        update_for_different_span_for_local_sd_ms_exer(aps, WEST);
    } else if (is_different_span_for_local_sd_ms_exer(aps, EAST)) {
        update_for_different_span_for_local_sd_ms_exer(aps, EAST);
    } else if (is_different_span_for_short_sd_ms_exer(aps, WEST)) {
        update_for_different_span_for_short_sd_ms_exer(aps, WEST);
    } else if (is_different_span_for_short_sd_ms_exer(aps, EAST)) {
        update_for_different_span_for_short_sd_ms_exer(aps, EAST);
    } else {
        assert(0);
    }
}

void to_start_up_for_coexist(struct aps_controller *aps)
{
    update_for_coexist_sd_ms_exer(aps);
    update_node_state(aps, IDLE);
    end_state_set(aps, END_START_UP);
}
