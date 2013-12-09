#include "ring_def.h"
#include "ring_state.h"
#include "ring_state_switch.h"
#include "ring_state_wtr.h"
#include "ring_state_primary.h"
#include "ring_aps_controller.h"
#include "ring_funcs.h"
#include <assert.h>

void unit_test_startup(struct aps_controller *aps) {
    //char k1 = 0x1;//not default and ais kbyte
    // init ring
    aps->ring_map[0] = 0;
    aps->ring_map[1] = 1;
    aps->ring_map[2] = 2;
    aps->ring_map[3] = 3;
    
    aps->node_id = 2;
    aps->neib_id[WEST] = 1;
    aps->neib_id[EAST] = 3;
    
    // not defatlt kbytes and not kbytes with ais add brq==nr;
    aps->drv_kbytes[WEST].k1.brcode = NR;
    aps->drv_kbytes[WEST].k2.src_node = 1;
    aps->drv_kbytes[EAST].k1.brcode = NR;
    aps->drv_kbytes[EAST].k2.src_node = 1;
    
    // init state machine
    end_state_init(aps);
    sw_state_init(aps);
    wtr_state_init(aps);
    prim_state_init(aps);
}

void unit_test_teardown(struct aps_controller *aps) {
    
}

void assert_tx_kbytes(const struct aps_controller *aps, int side, int brq, int dest, int src, int pth, int stat) {
    assert(aps->cur_kbytes[side].k1.brcode == brq);
    assert(aps->cur_kbytes[side].k1.dest_node == dest);
    assert(aps->cur_kbytes[side].k2.src_node == src);
    assert(aps->cur_kbytes[side].k2.path == pth);
    assert(aps->cur_kbytes[side].k2.status == stat);
}
