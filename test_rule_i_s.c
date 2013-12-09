#include "ring_def.h"
#include "ring_state.h"
#include "ring_state_switch.h"
#include "ring_state_wtr.h"
#include "ring_state_primary.h"
#include "ring_aps_controller.h"
#include "ring_funcs.h"
#include "unit_test_if.h"
#include <stdio.h>
//rule I-S#1b from idle to switching

void sd_head_from_idle_to_switching(struct aps_controller *aps) {
    aps->is_ne_ready = 1;
    // recv short sd brq, keep idle
    UPDATE_RX_KBYTES(EAST, SD, MY_NODE_ID, EAST_NODE_ID, SHORT_PATH, IDLE_STATUS);
    prim_state_run(aps);
    assert_tx_kbytes(aps, WEST, SD, EAST_NODE_ID, MY_NODE_ID, LONG_PATH, IDLE_ACK_STATUS);
    assert_tx_kbytes(aps, EAST, RR, EAST_NODE_ID, MY_NODE_ID, SHORT_PATH, IDLE_STATUS);
    
    // recv long sd brq, do br
    UPDATE_RX_KBYTES(WEST, SD, MY_NODE_ID, EAST_NODE_ID, LONG_PATH, BR_STATUS);
    prim_state_run(aps);
    assert_tx_kbytes(aps, WEST, SD, EAST_NODE_ID, MY_NODE_ID, LONG_PATH, NOT_IDLE_ACK_STATUS);
    assert_tx_kbytes(aps, EAST, RR, EAST_NODE_ID, MY_NODE_ID, SHORT_PATH, BR_STATUS);
    
    // recv long sd brq, do sw
    UPDATE_RX_KBYTES(WEST, SD, MY_NODE_ID, EAST_NODE_ID, LONG_PATH, BRSW_STATUS);
    prim_state_run(aps);
    assert_tx_kbytes(aps, WEST, SD, EAST_NODE_ID, MY_NODE_ID, LONG_PATH, NOT_IDLE_ACK_STATUS);
    assert_tx_kbytes(aps, EAST, RR, EAST_NODE_ID, MY_NODE_ID, SHORT_PATH, BRSW_STATUS);
    
    UPDATE_RX_KBYTES(WEST, NR, MY_NODE_ID, WEST_NODE_ID, SHORT_PATH, IDLE_STATUS);
    UPDATE_RX_KBYTES(EAST, NR, MY_NODE_ID, EAST_NODE_ID, SHORT_PATH, IDLE_STATUS);
}

void sd_tail_from_idle_to_switching(struct aps_controller *aps) {
    aps->is_ne_ready = 1;
    UPDATE_DQ(WEST, DQ_SD);
    prim_state_run(aps);
    assert_tx_kbytes(aps, WEST, SD, WEST_NODE_ID, MY_NODE_ID, SHORT_PATH, IDLE_STATUS);
    assert_tx_kbytes(aps, EAST, SD, WEST_NODE_ID, MY_NODE_ID, LONG_PATH, IDLE_STATUS);
    
    // recv long sd brq, do br
    UPDATE_RX_KBYTES(EAST, SD, MY_NODE_ID, WEST_NODE_ID, LONG_PATH, IDLE_ACK_STATUS);
    prim_state_run(aps);
    assert_tx_kbytes(aps, WEST, SD, WEST_NODE_ID, MY_NODE_ID, SHORT_PATH, BR_STATUS);
    assert_tx_kbytes(aps, EAST, SD, WEST_NODE_ID, MY_NODE_ID, LONG_PATH, BR_STATUS);
    
    // recv long sd brq, do sw
    UPDATE_RX_KBYTES(EAST, SD, MY_NODE_ID, WEST_NODE_ID, LONG_PATH, NOT_IDLE_ACK_STATUS);
    prim_state_run(aps);
    assert_tx_kbytes(aps, WEST, SD, WEST_NODE_ID, MY_NODE_ID, SHORT_PATH, BRSW_STATUS);
    assert_tx_kbytes(aps, EAST, SD, WEST_NODE_ID, MY_NODE_ID, LONG_PATH, BRSW_STATUS);
    
    UPDATE_DQ(WEST, DQ_CLEAR);
    UPDATE_RX_KBYTES(WEST, NR, MY_NODE_ID, WEST_NODE_ID, SHORT_PATH, IDLE_STATUS);
    UPDATE_RX_KBYTES(EAST, NR, MY_NODE_ID, EAST_NODE_ID, SHORT_PATH, IDLE_STATUS);
}

void sf_head_from_idle_to_switching(struct aps_controller *aps) {
    aps->is_ne_ready = 1;
    // recv short sd brq, keep idle
    UPDATE_RX_KBYTES(EAST, SF, MY_NODE_ID, EAST_NODE_ID, SHORT_PATH, IDLE_STATUS);
    prim_state_run(aps);
    assert_tx_kbytes(aps, WEST, SF, EAST_NODE_ID, MY_NODE_ID, LONG_PATH, IDLE_ACK_STATUS);
    assert_tx_kbytes(aps, EAST, RR, EAST_NODE_ID, MY_NODE_ID, SHORT_PATH, IDLE_STATUS);
    
    // recv long sd brq, do brsw
    UPDATE_RX_KBYTES(WEST, SD, MY_NODE_ID, EAST_NODE_ID, LONG_PATH, BRSW_STATUS);
    prim_state_run(aps);
    assert_tx_kbytes(aps, WEST, SF, EAST_NODE_ID, MY_NODE_ID, LONG_PATH, NOT_IDLE_ACK_STATUS);
    assert_tx_kbytes(aps, EAST, RR, EAST_NODE_ID, MY_NODE_ID, SHORT_PATH, BRSW_STATUS);
    
    UPDATE_RX_KBYTES(WEST, NR, MY_NODE_ID, WEST_NODE_ID, SHORT_PATH, IDLE_STATUS);
    UPDATE_RX_KBYTES(EAST, NR, MY_NODE_ID, EAST_NODE_ID, SHORT_PATH, IDLE_STATUS);
}

void sf_tail_from_idle_to_switching(struct aps_controller *aps) {
    aps->is_ne_ready = 1;
    // recv short sd brq, keep idle
    UPDATE_DQ(WEST, DQ_SF);
    prim_state_run(aps);
    assert_tx_kbytes(aps, WEST, SF, WEST_NODE_ID, MY_NODE_ID, SHORT_PATH, RDI_STATUS);
    assert_tx_kbytes(aps, EAST, SF, WEST_NODE_ID, MY_NODE_ID, LONG_PATH, IDLE_STATUS);
    
    // recv long sd brq, do brsw
    UPDATE_RX_KBYTES(EAST, SD, MY_NODE_ID, WEST_NODE_ID, LONG_PATH, NOT_IDLE_ACK_STATUS);
    prim_state_run(aps);
    assert_tx_kbytes(aps, WEST, SF, WEST_NODE_ID, MY_NODE_ID, SHORT_PATH, RDI_STATUS);
    assert_tx_kbytes(aps, EAST, SF, WEST_NODE_ID, MY_NODE_ID, LONG_PATH, BRSW_STATUS);
    
    UPDATE_DQ(WEST, DQ_CLEAR);
    UPDATE_RX_KBYTES(WEST, NR, MY_NODE_ID, WEST_NODE_ID, SHORT_PATH, IDLE_STATUS);
    UPDATE_RX_KBYTES(EAST, NR, MY_NODE_ID, EAST_NODE_ID, SHORT_PATH, IDLE_STATUS);
}

void sf_head_dq_clear_wtr_timeout(struct aps_controller *aps) {
    aps->is_ne_ready = 1;
    // recv short sd brq, keep idle
    UPDATE_RX_KBYTES(EAST, SF, MY_NODE_ID, EAST_NODE_ID, SHORT_PATH, IDLE_STATUS);
    prim_state_run(aps);
    assert_tx_kbytes(aps, WEST, SF, EAST_NODE_ID, MY_NODE_ID, LONG_PATH, IDLE_ACK_STATUS);
    assert_tx_kbytes(aps, EAST, RR, EAST_NODE_ID, MY_NODE_ID, SHORT_PATH, IDLE_STATUS);
    
    // recv long sd brq, do brsw
    UPDATE_RX_KBYTES(WEST, SD, MY_NODE_ID, EAST_NODE_ID, LONG_PATH, BRSW_STATUS);
    prim_state_run(aps);
    assert_tx_kbytes(aps, WEST, SF, EAST_NODE_ID, MY_NODE_ID, LONG_PATH, NOT_IDLE_ACK_STATUS);
    assert_tx_kbytes(aps, EAST, RR, EAST_NODE_ID, MY_NODE_ID, SHORT_PATH, BRSW_STATUS);
    
    // recv wtr
    UPDATE_RX_KBYTES(EAST, WTR, MY_NODE_ID, EAST_NODE_ID, SHORT_PATH, BRSW_STATUS);
    UPDATE_RX_KBYTES(WEST, WTR, MY_NODE_ID, EAST_NODE_ID, LONG_PATH, BRSW_STATUS);
    prim_state_run(aps);
    assert_tx_kbytes(aps, WEST, WTR, EAST_NODE_ID, MY_NODE_ID, LONG_PATH, BRSW_STATUS);
    assert_tx_kbytes(aps, EAST, RR, EAST_NODE_ID, MY_NODE_ID, SHORT_PATH, BRSW_STATUS);
    
    // wtr timeout
    UPDATE_RX_KBYTES(EAST, NR, MY_NODE_ID, EAST_NODE_ID, SHORT_PATH, BR_STATUS);
    UPDATE_RX_KBYTES(WEST, NR, MY_NODE_ID, EAST_NODE_ID, LONG_PATH, BR_STATUS);
    
    // send nr/idle both sides
    prim_state_run(aps);
    assert_tx_kbytes(aps, WEST, NR, WEST_NODE_ID, MY_NODE_ID, SHORT_PATH, IDLE_STATUS);
    assert_tx_kbytes(aps, EAST, NR, EAST_NODE_ID, MY_NODE_ID, SHORT_PATH, IDLE_STATUS);
}

void sf_tail_dq_clear_wtr_timeout(struct aps_controller *aps) {
    aps->is_ne_ready = 1;
    // recv short sd brq, keep idle
    UPDATE_DQ(WEST, DQ_SF);
    prim_state_run(aps);
    assert_tx_kbytes(aps, WEST, SF, WEST_NODE_ID, MY_NODE_ID, SHORT_PATH, RDI_STATUS);
    assert_tx_kbytes(aps, EAST, SF, WEST_NODE_ID, MY_NODE_ID, LONG_PATH, IDLE_STATUS);
    
    // recv long sd brq, do brsw
    UPDATE_RX_KBYTES(EAST, SD, MY_NODE_ID, WEST_NODE_ID, LONG_PATH, NOT_IDLE_ACK_STATUS);
    UPDATE_RX_KBYTES(WEST, RR, MY_NODE_ID, WEST_NODE_ID, SHORT_PATH, BRSW_STATUS);
    prim_state_run(aps);
    assert_tx_kbytes(aps, WEST, SF, WEST_NODE_ID, MY_NODE_ID, SHORT_PATH, RDI_STATUS);
    assert_tx_kbytes(aps, EAST, SF, WEST_NODE_ID, MY_NODE_ID, LONG_PATH, BRSW_STATUS);
    
    UPDATE_DQ(WEST, DQ_CLEAR);
    prim_state_run(aps);
    assert_tx_kbytes(aps, WEST, WTR, WEST_NODE_ID, MY_NODE_ID, SHORT_PATH, BRSW_STATUS);
    assert_tx_kbytes(aps, EAST, WTR, WEST_NODE_ID, MY_NODE_ID, LONG_PATH, BRSW_STATUS);
    
    aps->is_wtr_timeout = 1;
    aps->is_wtr_start = 1;
    prim_state_run(aps);
    assert_tx_kbytes(aps, WEST, NR, WEST_NODE_ID, MY_NODE_ID, SHORT_PATH, BR_STATUS);
    assert_tx_kbytes(aps, EAST, NR, WEST_NODE_ID, MY_NODE_ID, LONG_PATH, BR_STATUS);
    
    UPDATE_RX_KBYTES(EAST, NR, MY_NODE_ID, EAST_NODE_ID, SHORT_PATH, IDLE_STATUS);
    UPDATE_RX_KBYTES(WEST, NR, MY_NODE_ID, WEST_NODE_ID, SHORT_PATH, IDLE_STATUS);
    prim_state_run(aps);
    assert_tx_kbytes(aps, WEST, NR, WEST_NODE_ID, MY_NODE_ID, SHORT_PATH, IDLE_STATUS);
    assert_tx_kbytes(aps, EAST, NR, EAST_NODE_ID, MY_NODE_ID, SHORT_PATH, IDLE_STATUS);
    
    UPDATE_RX_KBYTES(WEST, NR, MY_NODE_ID, WEST_NODE_ID, SHORT_PATH, IDLE_STATUS);
    UPDATE_RX_KBYTES(EAST, NR, MY_NODE_ID, EAST_NODE_ID, SHORT_PATH, IDLE_STATUS);
}

// Rule I-S#4 switching node rx default code from short side, take no action.

// Rule I-S#5 (see figure I.5 node failure) when a swthing node which do no brsw recv long
// brq from both sides to it, take no action.
// in figrue I.5, use isolate node rule, the node will take no action also. so, I-S#5 is unnecessary?

// Rule I-S#6 a switching node recv long brq sending by itself from both sides, change to idle state.
void swtiching_node_rx_brqs_it_tx_from_both_sides(struct aps_controller *aps) {
    aps->is_ne_ready = 1;
    prim_state_run(aps);
    
    // node fail
    UPDATE_RX_KBYTES(WEST, SF, MY_NODE_ID, WEST_NODE_ID, SHORT_PATH, RDI_STATUS);
    UPDATE_RX_KBYTES(EAST, SF, MY_NODE_ID, EAST_NODE_ID, SHORT_PATH, RDI_STATUS);
    prim_state_run(aps);
    assert_tx_kbytes(aps, WEST, SF, EAST_NODE_ID, MY_NODE_ID, LONG_PATH, IDLE_STATUS);
    assert_tx_kbytes(aps, EAST, SF, WEST_NODE_ID, MY_NODE_ID, LONG_PATH, IDLE_STATUS);
    
    // rx brqs it sourcing both sides
    UPDATE_RX_KBYTES(WEST, SF, WEST_NODE_ID, MY_NODE_ID, LONG_PATH, IDLE_STATUS);
    UPDATE_RX_KBYTES(EAST, SF, EAST_NODE_ID, MY_NODE_ID, LONG_PATH, IDLE_STATUS);
    prim_state_run(aps);
    assert_tx_kbytes(aps, WEST, NR, WEST_NODE_ID, MY_NODE_ID, SHORT_PATH, IDLE_STATUS);
    assert_tx_kbytes(aps, EAST, NR, EAST_NODE_ID, MY_NODE_ID, SHORT_PATH, IDLE_STATUS);
    
    UPDATE_RX_KBYTES(WEST, NR, MY_NODE_ID, WEST_NODE_ID, SHORT_PATH, IDLE_STATUS);
    UPDATE_RX_KBYTES(EAST, NR, MY_NODE_ID, EAST_NODE_ID, SHORT_PATH, IDLE_STATUS);
}

// Rule I-S#7 two direction RR recovery
void switching_node_rx_and_tx_rr_over_same_span(struct aps_controller *aps) {
    aps->is_ne_ready = 1;
    // recv short sd brq, keep idle
    UPDATE_RX_KBYTES(EAST, SD, MY_NODE_ID, EAST_NODE_ID, SHORT_PATH, IDLE_STATUS);
    prim_state_run(aps);
    assert_tx_kbytes(aps, WEST, SD, EAST_NODE_ID, MY_NODE_ID, LONG_PATH, IDLE_ACK_STATUS);
    assert_tx_kbytes(aps, EAST, RR, EAST_NODE_ID, MY_NODE_ID, SHORT_PATH, IDLE_STATUS);
    
    // recv long sd brq, do br
    UPDATE_RX_KBYTES(WEST, SD, MY_NODE_ID, EAST_NODE_ID, LONG_PATH, BR_STATUS);
    prim_state_run(aps);
    assert_tx_kbytes(aps, WEST, SD, EAST_NODE_ID, MY_NODE_ID, LONG_PATH, NOT_IDLE_ACK_STATUS);
    assert_tx_kbytes(aps, EAST, RR, EAST_NODE_ID, MY_NODE_ID, SHORT_PATH, BR_STATUS);
    
    // recv long sd brq, do sw
    UPDATE_RX_KBYTES(WEST, SD, MY_NODE_ID, EAST_NODE_ID, LONG_PATH, BRSW_STATUS);
    prim_state_run(aps);
    assert_tx_kbytes(aps, WEST, SD, EAST_NODE_ID, MY_NODE_ID, LONG_PATH, NOT_IDLE_ACK_STATUS);
    assert_tx_kbytes(aps, EAST, RR, EAST_NODE_ID, MY_NODE_ID, SHORT_PATH, BRSW_STATUS);
    
    // recv rr over same span
    UPDATE_RX_KBYTES(EAST, RR, MY_NODE_ID, EAST_NODE_ID, SHORT_PATH, IDLE_STATUS);
    prim_state_run(aps);
    assert_tx_kbytes(aps, WEST, WTR, EAST_NODE_ID, MY_NODE_ID, LONG_PATH, BRSW_STATUS);
    assert_tx_kbytes(aps, EAST, WTR, EAST_NODE_ID, MY_NODE_ID, SHORT_PATH, BRSW_STATUS);
}


static struct aps_controller s_controller;
void swtich_idle_transition_test_run(void) {
    unit_test_startup(&s_controller);
    sd_head_from_idle_to_switching(&s_controller);
    unit_test_teardown(&s_controller);
    
    unit_test_startup(&s_controller);
    sf_head_from_idle_to_switching(&s_controller);
    unit_test_teardown(&s_controller);
    
    unit_test_startup(&s_controller);
    sd_tail_from_idle_to_switching(&s_controller);
    unit_test_teardown(&s_controller);
    
    unit_test_startup(&s_controller);
    sf_tail_from_idle_to_switching(&s_controller);
    unit_test_teardown(&s_controller);
    
    unit_test_startup(&s_controller);
    sf_tail_dq_clear_wtr_timeout(&s_controller);
    unit_test_teardown(&s_controller);
    
    unit_test_startup(&s_controller);
    sf_head_dq_clear_wtr_timeout(&s_controller);
    unit_test_teardown(&s_controller);
    
    unit_test_startup(&s_controller);
    swtiching_node_rx_brqs_it_tx_from_both_sides(&s_controller);
    unit_test_teardown(&s_controller);
    
    unit_test_startup(&s_controller);
    switching_node_rx_and_tx_rr_over_same_span(&s_controller);
    unit_test_teardown(&s_controller);
    
    printf("nswtich_idle_transition_test_run() okkkk!");
}
