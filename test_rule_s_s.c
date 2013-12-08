#include "ring_def.h"
#include "ring_state.h"
#include "ring_state_switch.h"
#include "ring_state_wtr.h"
#include "ring_state_primary.h"
#include "ring_aps_controller.h"
#include "ring_funcs.h"
#include "unit_test_if.h"

#include <stdio.h>
// attention: sf occur when sd br status, it should upgrade to sf br status
// ie. low middle status could be upgraded to higher middle status
// rule S-S#1c
void sd_tail_upgrade_to_sf_tail(struct aps_controller *aps)
{
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
    
    // new dq sf
    UPDATE_DQ(WEST, DQ_SF);
    prim_state_run(aps);
    assert_tx_kbytes(aps, WEST, SF, WEST_NODE_ID, MY_NODE_ID, SHORT_PATH, RDI_STATUS);
    assert_tx_kbytes(aps, EAST, SF, WEST_NODE_ID, MY_NODE_ID, LONG_PATH, BRSW_STATUS);
    
    // clear
    UPDATE_DQ(WEST, DQ_CLEAR);
    UPDATE_RX_KBYTES(WEST, NR, MY_NODE_ID, WEST_NODE_ID, SHORT_PATH, IDLE_STATUS);
    UPDATE_RX_KBYTES(EAST, NR, MY_NODE_ID, EAST_NODE_ID, SHORT_PATH, IDLE_STATUS);
}

void sd_head_upgrade_to_sf_tail(struct aps_controller *aps)
{
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
    
    // new dq sf
    UPDATE_DQ(EAST, DQ_SF);
    prim_state_run(aps);
    
    // upgrade to sf brsw
    assert_tx_kbytes(aps, EAST, SF, EAST_NODE_ID, MY_NODE_ID, SHORT_PATH, RDI_STATUS);
    assert_tx_kbytes(aps, WEST, SF, EAST_NODE_ID, MY_NODE_ID, LONG_PATH, BRSW_STATUS);
    
    // clear
    UPDATE_DQ(EAST, DQ_CLEAR);
    UPDATE_RX_KBYTES(WEST, NR, MY_NODE_ID, WEST_NODE_ID, SHORT_PATH, IDLE_STATUS);
    UPDATE_RX_KBYTES(EAST, NR, MY_NODE_ID, EAST_NODE_ID, SHORT_PATH, IDLE_STATUS);
}

void sd_tail_upgrade_to_sf_head(struct aps_controller *aps)
{
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
    
    // recv sf brq, upgrade to sf
    UPDATE_RX_KBYTES(WEST, SF, MY_NODE_ID, WEST_NODE_ID, SHORT_PATH, RDI_STATUS);
    prim_state_run(aps);
    assert_tx_kbytes(aps, WEST, RR, WEST_NODE_ID, MY_NODE_ID, SHORT_PATH, BRSW_STATUS);
    assert_tx_kbytes(aps, EAST, SF, WEST_NODE_ID, MY_NODE_ID, LONG_PATH, BRSW_STATUS);
    
    // clear
    UPDATE_DQ(WEST, DQ_CLEAR);
    UPDATE_RX_KBYTES(WEST, NR, MY_NODE_ID, WEST_NODE_ID, SHORT_PATH, IDLE_STATUS);
    UPDATE_RX_KBYTES(EAST, NR, MY_NODE_ID, EAST_NODE_ID, SHORT_PATH, IDLE_STATUS);
}

void sd_head_upgrade_to_sf_head(struct aps_controller *aps)
{
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
    
    // recv short sf brq, upgrade to sf
    UPDATE_RX_KBYTES(EAST, SF, MY_NODE_ID, EAST_NODE_ID, SHORT_PATH, RDI_STATUS);
    prim_state_run(aps);
    assert_tx_kbytes(aps, WEST, SF, EAST_NODE_ID, MY_NODE_ID, LONG_PATH, BRSW_STATUS);
    assert_tx_kbytes(aps, EAST, RR, EAST_NODE_ID, MY_NODE_ID, SHORT_PATH, BRSW_STATUS);
    
    // clear
    UPDATE_RX_KBYTES(WEST, NR, MY_NODE_ID, WEST_NODE_ID, SHORT_PATH, IDLE_STATUS);
    UPDATE_RX_KBYTES(EAST, NR, MY_NODE_ID, EAST_NODE_ID, SHORT_PATH, IDLE_STATUS);
}

// rule S-S#2f
void sd_tail_preemption_by_neib_sf_tail(struct aps_controller *aps)
{
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
    
    // new dq sf
    UPDATE_DQ(EAST, DQ_SF);
    prim_state_run(aps);
    assert_tx_kbytes(aps, WEST, SF, EAST_NODE_ID, MY_NODE_ID, LONG_PATH, IDLE_STATUS);
    assert_tx_kbytes(aps, EAST, SF, EAST_NODE_ID, MY_NODE_ID, SHORT_PATH, RDI_STATUS);
    
    // clear
    UPDATE_DQ(WEST, DQ_CLEAR);
    UPDATE_DQ(EAST, DQ_CLEAR);
    UPDATE_RX_KBYTES(WEST, NR, MY_NODE_ID, WEST_NODE_ID, SHORT_PATH, IDLE_STATUS);
    UPDATE_RX_KBYTES(EAST, NR, MY_NODE_ID, EAST_NODE_ID, SHORT_PATH, IDLE_STATUS);
}

void sd_tail_preemption_by_neib_sf_head(struct aps_controller *aps)
{
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
    
    // recv short sf brq, upgrade to sf
    UPDATE_RX_KBYTES(EAST, SF, MY_NODE_ID, EAST_NODE_ID, SHORT_PATH, RDI_STATUS);
    prim_state_run(aps);
    assert_tx_kbytes(aps, WEST, SF, EAST_NODE_ID, MY_NODE_ID, LONG_PATH, IDLE_ACK_STATUS);
    assert_tx_kbytes(aps, EAST, RR, EAST_NODE_ID, MY_NODE_ID, SHORT_PATH, IDLE_STATUS);
    
    // clear
    UPDATE_DQ(WEST, DQ_CLEAR);
    UPDATE_RX_KBYTES(WEST, NR, MY_NODE_ID, WEST_NODE_ID, SHORT_PATH, IDLE_STATUS);
    UPDATE_RX_KBYTES(EAST, NR, MY_NODE_ID, EAST_NODE_ID, SHORT_PATH, IDLE_STATUS);
}

void sd_head_preemption_by_neib_sf_tail(struct aps_controller *aps)
{
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
    
    // new dq sf
    UPDATE_DQ(WEST, DQ_SF);
    prim_state_run(aps);
    assert_tx_kbytes(aps, EAST, SF, WEST_NODE_ID, MY_NODE_ID, LONG_PATH, IDLE_STATUS);
    assert_tx_kbytes(aps, WEST, SF, WEST_NODE_ID, MY_NODE_ID, SHORT_PATH, RDI_STATUS);
    
    // clear
    UPDATE_DQ(WEST, DQ_CLEAR);
    UPDATE_RX_KBYTES(WEST, NR, MY_NODE_ID, WEST_NODE_ID, SHORT_PATH, IDLE_STATUS);
    UPDATE_RX_KBYTES(EAST, NR, MY_NODE_ID, EAST_NODE_ID, SHORT_PATH, IDLE_STATUS);
    
}

void sd_head_preemption_by_neib_sf_head(struct aps_controller *aps)
{
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
    
    // recv short sf brq, upgrade to sf
    UPDATE_RX_KBYTES(WEST, SF, MY_NODE_ID, EAST_NODE_ID, SHORT_PATH, RDI_STATUS);
    prim_state_run(aps);
    assert_tx_kbytes(aps, WEST, RR, WEST_NODE_ID, MY_NODE_ID, SHORT_PATH, IDLE_STATUS);
    assert_tx_kbytes(aps, EAST, SF, WEST_NODE_ID, MY_NODE_ID, LONG_PATH, IDLE_ACK_STATUS);
    
    // clear
    UPDATE_RX_KBYTES(WEST, NR, MY_NODE_ID, WEST_NODE_ID, SHORT_PATH, IDLE_STATUS);
    UPDATE_RX_KBYTES(EAST, NR, MY_NODE_ID, EAST_NODE_ID, SHORT_PATH, IDLE_STATUS);
}

// rule S-S#3a
void tail_wtr_preemption_by_head_sf(struct aps_controller *aps)
{
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
    
    // recv short sf brq, upgrade to sf
    UPDATE_RX_KBYTES(WEST, SF, MY_NODE_ID, WEST_NODE_ID, SHORT_PATH, RDI_STATUS);
    prim_state_run(aps);
    assert_tx_kbytes(aps, WEST, RR, WEST_NODE_ID, MY_NODE_ID, SHORT_PATH, BRSW_STATUS);
    assert_tx_kbytes(aps, EAST, SF, WEST_NODE_ID, MY_NODE_ID, LONG_PATH, BRSW_STATUS);
    
    // clear
    UPDATE_RX_KBYTES(WEST, NR, MY_NODE_ID, WEST_NODE_ID, SHORT_PATH, IDLE_STATUS);
    UPDATE_RX_KBYTES(EAST, NR, MY_NODE_ID, EAST_NODE_ID, SHORT_PATH, IDLE_STATUS);
}

void tail_wtr_preemption_by_neib_head_sf(struct aps_controller *aps)
{
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
    
    // recv short sf brq, upgrade to sf
    UPDATE_RX_KBYTES(EAST, SF, MY_NODE_ID, EAST_NODE_ID, SHORT_PATH, RDI_STATUS);
    prim_state_run(aps);
    assert_tx_kbytes(aps, WEST, SF, EAST_NODE_ID, MY_NODE_ID, LONG_PATH, IDLE_ACK_STATUS);
    assert_tx_kbytes(aps, EAST, RR, EAST_NODE_ID, MY_NODE_ID, SHORT_PATH, IDLE_STATUS);
    
    // clear
    UPDATE_RX_KBYTES(WEST, NR, MY_NODE_ID, WEST_NODE_ID, SHORT_PATH, IDLE_STATUS);
    UPDATE_RX_KBYTES(EAST, NR, MY_NODE_ID, EAST_NODE_ID, SHORT_PATH, IDLE_STATUS);
}


void head_response_to_brq_wtr(struct aps_controller *aps)
{
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
    
    // recv wtr brq
    UPDATE_RX_KBYTES(EAST, WTR, MY_NODE_ID, EAST_NODE_ID, SHORT_PATH, BRSW_STATUS);
    UPDATE_RX_KBYTES(WEST, WTR, MY_NODE_ID, EAST_NODE_ID, LONG_PATH, BRSW_STATUS);
    prim_state_run(aps);
    
    assert_tx_kbytes(aps, WEST, WTR, EAST_NODE_ID, MY_NODE_ID, LONG_PATH, BRSW_STATUS);
    assert_tx_kbytes(aps, EAST, RR, EAST_NODE_ID, MY_NODE_ID, SHORT_PATH, BRSW_STATUS);
    
    // clear
    UPDATE_RX_KBYTES(WEST, NR, MY_NODE_ID, WEST_NODE_ID, SHORT_PATH, IDLE_STATUS);
    UPDATE_RX_KBYTES(EAST, NR, MY_NODE_ID, EAST_NODE_ID, SHORT_PATH, IDLE_STATUS);
    
}

// rule S-S#5
void switching_node_rx_brqs_from_neibs_both_sides(struct aps_controller *aps)
{
}

static struct aps_controller s_controller;
void switch_to_switch_test_run(void)
{
    unit_test_startup(&s_controller);
    sd_tail_upgrade_to_sf_tail(&s_controller);
    unit_test_teardown(&s_controller);
    
    unit_test_startup(&s_controller);
    sd_head_upgrade_to_sf_tail(&s_controller);
    unit_test_teardown(&s_controller);
    
    unit_test_startup(&s_controller);
    sd_tail_upgrade_to_sf_head(&s_controller);
    unit_test_teardown(&s_controller);
    
    unit_test_startup(&s_controller);
    sd_head_upgrade_to_sf_head(&s_controller);
    unit_test_teardown(&s_controller);
    
    unit_test_startup(&s_controller);
    sd_tail_preemption_by_neib_sf_tail(&s_controller);
    unit_test_teardown(&s_controller);
    
    unit_test_startup(&s_controller);
    sd_tail_preemption_by_neib_sf_head(&s_controller);
    unit_test_teardown(&s_controller);
    
    unit_test_startup(&s_controller);
    sd_head_preemption_by_neib_sf_tail(&s_controller);
    unit_test_teardown(&s_controller);
    
    unit_test_startup(&s_controller);
    sd_head_preemption_by_neib_sf_head(&s_controller);
    unit_test_teardown(&s_controller);
    
    unit_test_startup(&s_controller);
    tail_wtr_preemption_by_head_sf(&s_controller);
    unit_test_teardown(&s_controller);
    
    unit_test_startup(&s_controller);
    tail_wtr_preemption_by_neib_head_sf(&s_controller);
    unit_test_teardown(&s_controller);
    
    unit_test_startup(&s_controller);
    head_response_to_brq_wtr(&s_controller);
    unit_test_teardown(&s_controller);
    
    printf("nswitch_to_switch_test_run() okkkk!");
}
