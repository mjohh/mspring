#include "ring_def.h"
#include "ring_state.h"
#include "ring_state_switch.h"
#include "ring_state_wtr.h"
#include "ring_state_primary.h"
#include "ring_aps_controller.h"
#include "ring_funcs.h"
#include "unit_test_if.h"
#include <assert.h>
#include <stdio.h>

void startup_case(struct aps_controller *aps) {
    // before node ready, tx default code
    aps->is_ne_ready = 0;
    prim_state_run(aps);
    assert_tx_kbytes(aps, WEST, 0, 0, 0, 0, 0);
    assert_tx_kbytes(aps, EAST, 0, 0, 0, 0, 0);
}

void ne_ready_case(struct aps_controller *aps) {
    // before node ready, tx default code
    aps->is_ne_ready = 1;
    UPDATE_RX_KBYTES(WEST, SF, MY_NODE_ID, WEST_NODE_ID, SHORT_PATH, BRSW_STATUS);
    UPDATE_RX_KBYTES(EAST, SF, MY_NODE_ID, EAST_NODE_ID, SHORT_PATH, BRSW_STATUS);
    prim_state_run(aps);
    assert_tx_kbytes(aps, EAST, SF, WEST_NODE_ID, MY_NODE_ID, LONG_PATH, IDLE_STATUS);
    assert_tx_kbytes(aps, WEST, SF, EAST_NODE_ID, MY_NODE_ID, LONG_PATH, IDLE_STATUS);
    
    UPDATE_RX_KBYTES(EAST, SF, MY_NODE_ID, WEST_NODE_ID, LONG_PATH, BRSW_STATUS);
    UPDATE_RX_KBYTES(WEST, SF, MY_NODE_ID, EAST_NODE_ID, LONG_PATH, BRSW_STATUS);
    prim_state_run(aps);
    assert_tx_kbytes(aps, EAST, SF, WEST_NODE_ID, MY_NODE_ID, LONG_PATH, IDLE_STATUS);
    assert_tx_kbytes(aps, WEST, SF, EAST_NODE_ID, MY_NODE_ID, LONG_PATH, IDLE_STATUS);
    
    UPDATE_RX_KBYTES(EAST, SF, EAST_NODE_ID, MY_NODE_ID, LONG_PATH, IDLE_STATUS);
    UPDATE_RX_KBYTES(WEST, SF, WEST_NODE_ID, MY_NODE_ID, LONG_PATH, IDLE_STATUS);
    prim_state_run(aps);
    assert_tx_kbytes(aps, EAST, NR, EAST_NODE_ID, MY_NODE_ID, SHORT_PATH, IDLE_STATUS);
    assert_tx_kbytes(aps, WEST, NR, WEST_NODE_ID, MY_NODE_ID, SHORT_PATH, IDLE_STATUS);
}

void ne_start_up_neib_case(struct aps_controller *aps) {
    aps->is_ne_ready = 1;
    UPDATE_DQ(WEST, DQ_SF);
    prim_state_run(aps);
    assert_tx_kbytes(aps, WEST, SF, WEST_NODE_ID, MY_NODE_ID, SHORT_PATH, RDI_STATUS);
    assert_tx_kbytes(aps, EAST, SF, WEST_NODE_ID, MY_NODE_ID, LONG_PATH, IDLE_STATUS);
    UPDATE_DQ(WEST, DQ_CLEAR);
    
    UPDATE_RX_KBYTES(WEST, 0, 0, 0, 0, 0);
    UPDATE_RX_KBYTES(EAST, SF, WEST_NODE_ID, WEST_NODE_ID - 1, LONG_PATH, IDLE_STATUS);
    prim_state_run(aps);
    assert_tx_kbytes(aps, WEST, SF, WEST_NODE_ID, MY_NODE_ID, SHORT_PATH, RDI_STATUS);
    assert_tx_kbytes(aps, EAST, SF, WEST_NODE_ID, MY_NODE_ID, LONG_PATH, BRSW_STATUS);
    
    // WEST_NODE_ID-1 may be illegal, but do not affect test result
    UPDATE_RX_KBYTES(WEST, SF, WEST_NODE_ID - 1, WEST_NODE_ID, LONG_PATH, IDLE_STATUS);
    prim_state_run(aps);
    assert(aps->cur_prim_state == PRIM_PASS);
    
    UPDATE_RX_KBYTES(EAST, SF, MY_NODE_ID, WEST_NODE_ID, LONG_PATH, IDLE_STATUS);
    prim_state_run(aps);
    assert(aps->cur_prim_state == PRIM_PASS);
}

// sd:dq-->O                         (dq)
// sd:brq,short-->O                  (brq)
// sd:dq-->O<--sd:brq,short          (dq+brq, dq > brq)
// sf:dq-->O                         (dq, keep RDI state on short side)
// sf:dq-->O<--sd:dq                 (dq+dq, sf > sd)
// sf:brq,short-->O<--sf:brq,short   (brq+brq, send long brq on both sides, but do not brsw)
// sf:dq-->O<--sf:dq                 (dq+dq, corexist, send short brq both sides, but do not brsw)
// local brq sf,fs-->O<--other brq sf,fs   (diff span coexist)
// local brq sd,ms,exer-->O<--other brq sd,ms,exer   (diff span coexist)

void one_dq_case(struct aps_controller *aps) {
    aps->is_ne_ready = 1;
    UPDATE_RX_KBYTES(WEST, NR, MY_NODE_ID, WEST_NODE_ID, SHORT_PATH, IDLE_STATUS);
    UPDATE_RX_KBYTES(EAST, NR, MY_NODE_ID, EAST_NODE_ID, SHORT_PATH, IDLE_STATUS);
    UPDATE_DQ(WEST, DQ_SD);
    prim_state_run(aps);
    assert_tx_kbytes(aps, WEST, SD, WEST_NODE_ID, MY_NODE_ID, SHORT_PATH, IDLE_STATUS);
    assert_tx_kbytes(aps, EAST, SD, WEST_NODE_ID, MY_NODE_ID, LONG_PATH, IDLE_STATUS);
    UPDATE_DQ(WEST, DQ_CLEAR);
}

void one_short_brq_case(struct aps_controller *aps) {
    aps->is_ne_ready = 1;
    UPDATE_RX_KBYTES(WEST, SD, MY_NODE_ID, WEST_NODE_ID, SHORT_PATH, IDLE_STATUS);
    prim_state_run(aps);
    assert_tx_kbytes(aps, WEST, RR, WEST_NODE_ID, MY_NODE_ID, SHORT_PATH, IDLE_STATUS);
    assert_tx_kbytes(aps, EAST, SD, WEST_NODE_ID, MY_NODE_ID, LONG_PATH, IDLE_ACK_STATUS);
    UPDATE_RX_KBYTES(WEST, NR, MY_NODE_ID, WEST_NODE_ID, SHORT_PATH, IDLE_STATUS);
}

void one_dq_one_short_brq_case(struct aps_controller *aps) {
    aps->is_ne_ready = 1;
    UPDATE_DQ(WEST, DQ_SD);
    UPDATE_RX_KBYTES(EAST, SD, MY_NODE_ID, EAST_NODE_ID, SHORT_PATH, IDLE_STATUS);
    prim_state_run(aps);
    assert_tx_kbytes(aps, WEST, SD, WEST_NODE_ID, MY_NODE_ID, SHORT_PATH, IDLE_STATUS);
    assert_tx_kbytes(aps, EAST, SD, WEST_NODE_ID, MY_NODE_ID, LONG_PATH, IDLE_STATUS);
    UPDATE_DQ(WEST, DQ_CLEAR);
    UPDATE_RX_KBYTES(WEST, NR, MY_NODE_ID, WEST_NODE_ID, SHORT_PATH, IDLE_STATUS);
}

void one_dq_sf_case(struct aps_controller *aps) {
    aps->is_ne_ready = 1;
    UPDATE_DQ(WEST, DQ_SF);
    prim_state_run(aps);
    assert_tx_kbytes(aps, WEST, SF, WEST_NODE_ID, MY_NODE_ID, SHORT_PATH, RDI_STATUS);
    assert_tx_kbytes(aps, EAST, SF, WEST_NODE_ID, MY_NODE_ID, LONG_PATH, IDLE_STATUS);
    UPDATE_DQ(WEST, DQ_CLEAR);
}

void dq_sf_and_dq_sd_case(struct aps_controller *aps) {
    aps->is_ne_ready = 1;
    UPDATE_DQ(WEST, DQ_SF);
    UPDATE_DQ(EAST, DQ_SD);
    prim_state_run(aps);
    assert_tx_kbytes(aps, WEST, SF, WEST_NODE_ID, MY_NODE_ID, SHORT_PATH, RDI_STATUS);
    assert_tx_kbytes(aps, EAST, SF, WEST_NODE_ID, MY_NODE_ID, LONG_PATH, IDLE_STATUS);
    UPDATE_DQ(WEST, DQ_CLEAR);
    UPDATE_DQ(EAST, DQ_CLEAR);
}

void short_sf_and_short_sf_case(struct aps_controller *aps) {
    aps->is_ne_ready = 1;
    UPDATE_RX_KBYTES(WEST, SF, MY_NODE_ID, WEST_NODE_ID, SHORT_PATH, IDLE_STATUS);
    UPDATE_RX_KBYTES(EAST, SF, MY_NODE_ID, EAST_NODE_ID, SHORT_PATH, IDLE_STATUS);
    prim_state_run(aps);
    assert_tx_kbytes(aps, WEST, SF, EAST_NODE_ID, MY_NODE_ID, LONG_PATH, IDLE_STATUS);
    assert_tx_kbytes(aps, EAST, SF, WEST_NODE_ID, MY_NODE_ID, LONG_PATH, IDLE_STATUS);
    UPDATE_RX_KBYTES(WEST, NR, MY_NODE_ID, WEST_NODE_ID, SHORT_PATH, IDLE_STATUS);
    UPDATE_RX_KBYTES(EAST, NR, MY_NODE_ID, EAST_NODE_ID, SHORT_PATH, IDLE_STATUS);
}

void dq_sf_and_dq_sf_case(struct aps_controller *aps) {
    aps->is_ne_ready = 1;
    UPDATE_DQ(WEST, DQ_SF);
    UPDATE_DQ(EAST, DQ_SF);
    prim_state_run(aps);
    assert_tx_kbytes(aps, WEST, SF, WEST_NODE_ID, MY_NODE_ID, SHORT_PATH, RDI_STATUS);
    assert_tx_kbytes(aps, EAST, SF, EAST_NODE_ID, MY_NODE_ID, SHORT_PATH, RDI_STATUS);
    UPDATE_DQ(WEST, DQ_CLEAR);
    UPDATE_DQ(EAST, DQ_CLEAR);
}

void diff_span_coexist_case(struct aps_controller *aps) {
    aps->is_ne_ready = 1;
    UPDATE_RX_KBYTES(WEST, SF, MY_NODE_ID, WEST_NODE_ID, SHORT_PATH, IDLE_STATUS);
    UPDATE_RX_KBYTES(EAST, FS, MY_NODE_ID, EAST_NODE_ID, SHORT_PATH, IDLE_STATUS);
    prim_state_run(aps);
    assert_tx_kbytes(aps, WEST, FS, EAST_NODE_ID, MY_NODE_ID, LONG_PATH, IDLE_STATUS);
    assert_tx_kbytes(aps, EAST, SF, WEST_NODE_ID, MY_NODE_ID, LONG_PATH, IDLE_STATUS);
    
    UPDATE_RX_KBYTES(WEST, SD, MY_NODE_ID, WEST_NODE_ID, SHORT_PATH, IDLE_STATUS);
    UPDATE_RX_KBYTES(EAST, MS, MY_NODE_ID, EAST_NODE_ID, SHORT_PATH, IDLE_STATUS);
    prim_state_run(aps);
    assert_tx_kbytes(aps, WEST, MS, EAST_NODE_ID, MY_NODE_ID, LONG_PATH, IDLE_STATUS);
    assert_tx_kbytes(aps, EAST, SD, WEST_NODE_ID, MY_NODE_ID, LONG_PATH, IDLE_STATUS);
}

static struct aps_controller s_controller;

void switching_state_test_run(void) {
    unit_test_startup(&s_controller);
    //startup_case(&s_controller);
    //ne_ready_case(&s_controller);
    one_dq_case(&s_controller);
    one_short_brq_case(&s_controller);
    one_dq_one_short_brq_case(&s_controller);
    one_dq_sf_case(&s_controller);
    dq_sf_and_dq_sd_case(&s_controller);
    short_sf_and_short_sf_case(&s_controller);
    dq_sf_and_dq_sf_case(&s_controller);
    diff_span_coexist_case(&s_controller);
    unit_test_teardown(&s_controller);
    printf("nswitching_state_test_run() okkkk!");
}

void node_start_up_test_run(void) {
    unit_test_startup(&s_controller);
    startup_case(&s_controller);
    ne_ready_case(&s_controller);
    unit_test_teardown(&s_controller);
    printf("node_start_up_test_run() okkkk!");
}

void node_start_up_neib_test_run(void) {
    unit_test_startup(&s_controller);
    ne_start_up_neib_case(&s_controller);
    unit_test_teardown(&s_controller);
}
