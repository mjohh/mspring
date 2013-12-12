#include "ring_def.h"
#include "ring_state.h"
#include "ring_state_switch.h"
#include "ring_state_wtr.h"
#include "ring_state_primary.h"
#include "ring_aps_controller.h"
#include "ring_funcs.h"
#include "ring_dbg.h"
#include <assert.h>
#include <memory.h>

int aps_init(struct aps_controller *aps) {
    char k1 = DEFAULT_K1BYTE;
    char k2 = DEFAULT_K2BYTE;
    assert(aps);
    // init input
    memcpy(&DRV_KBYTES_K1(WEST), &k1, sizeof(k1));
    memcpy(&DRV_KBYTES_K2(WEST), &k2, sizeof(k2));
    memcpy(&DRV_KBYTES_K1(EAST), &k1, sizeof(k1));
    memcpy(&DRV_KBYTES_K2(EAST), &k2, sizeof(k2));
    aps->dq[WEST] = DQ_CLEAR;
    aps->dq[EAST] = DQ_CLEAR;
    aps->ext_cmd = EX_NONE;
    aps->ext_side = UNDEFINED_SIDE;
    aps->is_ne_ready = 0;
    aps->is_wtr_timeout = 0;
    // init sm
    end_state_init(aps);
    sw_state_init(aps);
    wtr_state_init(aps);
    prim_state_init(aps);
    return 0;
}

void aps_fini(struct aps_controller *aps) {
    assert(aps);
    prim_state_fini(aps);
    wtr_state_fini(aps);
    sw_state_fini(aps);
    end_state_fini(aps);
}

void aps_run(struct aps_controller *aps) {
    assert(aps);
    prim_state_run(aps);
}

void aps_input_kbyte(struct aps_controller *aps, enum side side, struct k1k2 *k1k2) {
    assert(aps && k1k2);
    assert(WEST == side || EAST == side);
    memcpy(&aps->drv_kbytes[side], k1k2, sizeof(*k1k2));
}

void aps_input_dq(struct aps_controller *aps, enum side side, enum dq dq) {
    assert(aps);
    assert(WEST == side || EAST == side);
    assert(dq >= DQ_CLEAR && dq < NUM_DQ_CODES);
    aps->dq[side] = dq;
}

void aps_input_ext_cmd(struct aps_controller *aps, enum side side, enum ext_cmd ext_cmd) {
    assert(aps);
    assert(WEST == side || EAST == side);
    assert(ext_cmd >= EX_NONE && ext_cmd < NUM_EXT_CMDS);
    aps->ext_cmd = ext_cmd;
    aps->ext_side = side;
}

void aps_input_ne_ready_flag(struct aps_controller *aps, int is_ne_ready) {
    assert(aps);
    aps->is_ne_ready = is_ne_ready;
}

void aps_input_wtr_timeout_flag(struct aps_controller *aps, int is_wtr_timeout) {
    assert(aps);
    aps->is_wtr_timeout = is_wtr_timeout;
}

void aps_output(struct aps_controller* aps,
               int (*send_kbyte)(int ringid, int slot, int port, struct k1k2 * k1k2),
               int (*switch)(int ringid, int slot[NUM_SIDES], int port[NUM_SIDES], enum node_state state),
               int (*start_wtr)(int ringid, int enable, int sec)) {
    assert(aps);
    if (output_update_hw) {
        output_update_hw(aps->ring_id, aps->slot, aps->port, aps->node_state);
    }
    if (output_set_kbyte) {
        output_set_kbyte(aps->ring_id, aps->slot[WEST], aps->port[WEST], &aps->cur_kbytes[WEST]);
        output_set_kbyte(aps->ring_id, aps->slot[EAST], aps->port[EAST], &aps->cur_kbytes[EAST]);
    }
    if (output_start_wtr) {
        output_start_wtr(aps->ring_id, aps->is_wtr_start, aps->wtr_time);
    }
}

int aps_cfg(struct aps_controller * aps,
            unsigned short ring_map[MAX_NODES_IN_RING],
            int nodes_num,
            int node_id,
            int ring_id,
            int slot[NUM_SIDES],
            int port[NUM_SIDES],
            int wtr_time) {
    int i;
    assert(aps && ring_map && slot && port);
    assert(nodes_num > 0 && nodes_num < MAX_NODES_IN_RING);
    
    for (i = 0; i < nodes_num; i++) {
        if (ring_map[i] == node_id)
            break;
    }
    if (i == nodes_num)
        return 1;
    aps->neib_id[WEST] = (0 == i ? ring_map[nodes_num-1] : ring_map[i-1]);
    aps->neib_id[EAST] = (nodes_num-1 == i ? ring_map[0] : ring_map[i+1]);
    
    for (i = 0; i < nodes_num; i++) {
        aps->ring_map[i] = ring_map[i];
    }
    aps->nodes_num = nodes_num;
    aps->node_id = node_id;
    aps->ring_id = ring_id;
    aps->slot[WEST] = slot[WEST];
    aps->slot[EAST] = slot[EAST];
    aps->port[WEST] = port[WEST];
    aps->port[EAST] = port[EAST];
    aps->wtr_time = wtr_time;
    return 0;
}
