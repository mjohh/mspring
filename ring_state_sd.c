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
#include <assert.h>

void sd_start_up_change(struct aps_controller *aps) {
    if (is_coexist_sd_ms_exer(aps)) {
        to_start_up_for_coexist(aps);
    } else {
        start_up_change(aps);
    }
}

void sd_idle_and_waiting_change(struct aps_controller *aps) {
    if (is_coexist_sd_ms_exer(aps)) {
        to_start_up_for_coexist(aps);
    } else {
        idle_and_waiting_change(aps);
    }
}

void sd_br_and_waiting_change(struct aps_controller *aps) {
    if (is_coexist_sd_ms_exer(aps)) {
        to_start_up_for_coexist(aps);
    } else {
        br_and_waiting_change(aps);
    }
}

void sd_brsw_change(struct aps_controller *aps) {
    if (is_coexist_sd_ms_exer(aps)) {
        to_start_up_for_coexist(aps);
    } else {
        brsw_change(aps);
    }
}

void sd_state_exit(struct aps_controller *aps) {
    aps->cur_end_state = END_START_UP;
}

void sd_state_run(struct aps_controller *aps) {
    assert(aps);
    switch (aps->cur_end_state) {
        case END_START_UP:
            sd_start_up_change(aps);
            break;
        case END_IDLE_AND_WAITING:
            sd_idle_and_waiting_change(aps);
            break;
        case END_BR_AND_WAITING:
            sd_br_and_waiting_change(aps);
            break;
        case END_BRSW:
            sd_brsw_change(aps);
            break;
        default:
            assert(0);
            break;
    }
    //ring_print(aps->node_id, "   exit sd, state = %sn", end_state_name(aps->cur_end_state));
}
