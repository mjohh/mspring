#include "ring_def.h"
#include "ring_state.h"
#include "ring_state_switch.h"
#include "ring_state_wtr.h"
#include "ring_state_primary.h"
#include "ring_aps_controller.h"
#include "ring_funcs.h"
#include "unit_test_if.h"

#include <stdio.h>
#include <assert.h>

// rule P#2 keep in passthru
void passthru_node_recv_same_pri_brq_to_it(struct aps_controller *aps)
{
    aps->is_ne_ready = 1;
    
    // the brq is illegal but could cause passthru
    UPDATE_RX_KBYTES(EAST, SD, WEST_NODE_ID, EAST_NODE_ID, LONG_PATH, IDLE_STATUS);
    prim_state_run(aps);
    
    // into passthru
    assert(aps->cur_prim_state == PRIM_PASS);
    
    UPDATE_RX_KBYTES(WEST, SD, MY_NODE_ID, EAST_NODE_ID, LONG_PATH, IDLE_STATUS);
    prim_state_run(aps);
    assert(aps->cur_prim_state == PRIM_PASS);
}

static struct aps_controller s_controller;
void passthru_test_run(void)
{
    unit_test_startup(&s_controller);
    passthru_node_recv_same_pri_brq_to_it(&s_controller);
    unit_test_teardown(&s_controller);
    
    printf("npassthru_test_run() okkkk!");
}
