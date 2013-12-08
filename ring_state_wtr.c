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
#include "ring_dbg.h"

#include <assert.h>

extern void end_state_set(struct aps_controller *aps, enum end_state_id next_state);

static void _stop_timer(struct aps_controller *aps)
{
    aps->is_wtr_timeout = 0;
    aps->is_wtr_start = 0;
}

static void _tail_to_br_and_waiting_state(struct aps_controller *aps)
{
    enum side other_side = OTHER_SIDE(aps->short_side);
    UPDATE_CURRENT_KBYTES(aps->short_side, NR, NEIB_NODE_ID(aps->short_side), MY_NODE_ID, SHORT_PATH, BR_STATUS);
    UPDATE_CURRENT_KBYTES(other_side, NR, NEIB_NODE_ID(aps->short_side), MY_NODE_ID, LONG_PATH, BR_STATUS);
    end_state_set(aps, END_BR_AND_WAITING);
}

static void _to_brsw_and_waiting_state(struct aps_controller *aps, enum brq_code short_brq, enum brq_code long_brq)
{
    enum side other_side = OTHER_SIDE(aps->short_side);
    UPDATE_CURRENT_KBYTES(aps->short_side, short_brq, NEIB_NODE_ID(aps->short_side), MY_NODE_ID, SHORT_PATH, BRSW_STATUS);
    UPDATE_CURRENT_KBYTES(other_side, long_brq, NEIB_NODE_ID(aps->short_side), MY_NODE_ID, LONG_PATH, BRSW_STATUS);
    end_state_set(aps, END_BRSW);
}

static void _to_wtr_idle_state(struct aps_controller *aps)
{
    UPDATE_CURRENT_KBYTES(WEST, NR, NEIB_NODE_ID(WEST), MY_NODE_ID, SHORT_PATH, IDLE_STATUS);
    UPDATE_CURRENT_KBYTES(EAST, NR, NEIB_NODE_ID(EAST), MY_NODE_ID, SHORT_PATH, IDLE_STATUS);
    update_node_state(aps, IDLE);
    end_state_set(aps, END_START_UP);
}

static void wtr_timer_start(struct aps_controller *aps)
{
    aps->is_wtr_timeout = 0;
    aps->is_wtr_start = 1;
}

///////////////////////////////////////////////////////////////////////////////

static void wtr_brsw_state_change(struct aps_controller *aps)
{
    if (is_tail_end(aps)) {
        if (SW_SD == aps->cur_sw_state || SW_SF == aps->cur_sw_state) {
            if (!aps->is_wtr_start) {
                wtr_timer_start(aps);
                _to_brsw_and_waiting_state(aps, WTR, WTR);
            } else if (aps->is_wtr_timeout) {
                _tail_to_br_and_waiting_state(aps);
                aps->is_wtr_start = 0;
                aps->is_wtr_timeout = 0;
                //ring_print(aps->node_id, "_to_br_and_waiting_staten");
            }
        } else {
            _tail_to_br_and_waiting_state(aps);
        }
    } else			// head end
    {
        enum brq_code short_brq = get_kbyte_brq(aps, aps->short_side);
        enum side other_side = OTHER_SIDE(aps->short_side);
        enum brq_code other_side_brq = get_kbyte_brq(aps, other_side);
        if (WTR == short_brq) {
            // keep wtr brsw state
            _to_brsw_and_waiting_state(aps, RR, WTR);
        }			// long nr/br
        else if (NR == other_side_brq && DRV_KBYTES_STATUS(other_side) == BR_STATUS) {
            _to_wtr_idle_state(aps);
            //ring_print(aps->node_id, "_to_wtr_idle_staten");
        }
    }
}

static void wtr_br_and_waiting_state_change(struct aps_controller *aps)
{
    enum side other_side = OTHER_SIDE(aps->short_side);
    enum brq_code other_side_brq = get_kbyte_brq(aps, other_side);
    
    if (NR == other_side_brq && DRV_KBYTES_STATUS(other_side) == BR_STATUS) {
        _to_wtr_idle_state(aps);
        //ring_print(aps->node_id, "_to_wtr_idle_staten");
    }
}

///////////////////////////////////////////////////////////////////////////////

void wtr_state_init(struct aps_controller *aps)
{
    aps->is_wtr_start = 0;
    aps->is_wtr_timeout = 0;
}

void wtr_state_fini(struct aps_controller *aps)
{
    // TODO:
}

void wtr_state_run(struct aps_controller *aps)
{
    assert(aps);
    
    switch (aps->cur_end_state) {
        case END_START_UP:
            // lop clean will enter here
            _to_wtr_idle_state(aps);
            break;
        case END_BR_AND_WAITING:
            // when end switch was broken, wtr will start from here
            wtr_br_and_waiting_state_change(aps);
            break;
        case END_BRSW:
            wtr_brsw_state_change(aps);
            break;
        default:
            assert(0);
            break;
    }
    //ring_print(aps->node_id, "   exit wtr, state = %sn", end_state_name(aps->cur_end_state));
}

void wtr_state_exit(struct aps_controller *aps)
{
    // end state will clean only by switch state
    // for when wtr to switch, wtr need exit firstly, and reuse end state by switch.
    _stop_timer(aps);
}
