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
    assert(aps);
    // init input
    memset(aps->drv_kbytes, 0, sizeof(aps->drv_kbytes));
    aps->dq[WEST] = DQ_CLEAR;
    aps->dq[EAST] = DQ_CLEAR;
    aps->ext_cmd = EX_NONE;
    aps->ext_side = UNDEFINED_SIDE;
    aps->is_ne_ready = 0;
    aps->is_wtr_timeout = 0;
    // init output
    memset(aps->cur_kbytes, 0, sizeof(aps->cur_kbytes));
    aps->is_wtr_start = 0;
    aps->node_state = IDLE;
    // init input filter
    memset(aps->drv_kbytes_filter, 0, sizeof(aps->drv_kbytes_filter));
    aps->dq_filter[WEST] = DQ_CLEAR;
    aps->dq_filter[EAST] = DQ_CLEAR;
    aps->ext_cmd_filter = EX_NONE;
    aps->ext_side_filter = UNDEFINED_SIDE;
    aps->is_ne_ready_filter = 0;
    aps->is_wtr_timeout_filter = 0;
    // int output filter
    memset(aps->cur_kbytes_filter, 0, sizeof(aps->cur_kbytes_filter));
    aps->is_wtr_start_filter = 0;
    aps->node_state_filter = IDLE;
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

int input_changed(struct aps_controller *aps) {
    int changed = 0;
    assert(aps);
    if (memcmp(aps->drv_kbytes, aps->drv_kbytes_filter, sizeof(aps->drv_kbytes))) {
        memcpy(aps->drv_kbytes, aps->drv_kbytes_filter, sizeof(aps->drv_kbytes));
        changed++;
    }
    if (memcmp(aps->dq, aps->dq_filter, sizeof(aps->dq))) {
        memcpy(aps->dq, aps->dq_filter, sizeof(aps->dq));
        changed++;
    }
    if (aps->ext_cmd != aps->ext_cmd_filter) {
        aps->ext_cmd_filter = aps->ext_cmd;
        changed++;
    }
    if (aps->ext_side != aps->ext_side_filter) {
        aps->ext_side_filter = aps->ext_side;
        changed++;
    }
    if (aps->is_ne_ready != aps->is_ne_ready_filter) {
        aps->is_ne_ready_filter = aps->is_ne_ready;
        changed++;
    }
    if (aps->is_wtr_timeout != aps->is_wtr_timeout_filter) {
        aps->is_wtr_timeout_filter = aps->is_wtr_timeout;
        changed++;
    }
    return changed;
}

void aps_run(struct aps_controller *aps) {
    assert(aps);
    if (input_changed(aps)) {
        prim_state_run(aps);
    }
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
               void (*sendkbyte)(int ringid, int slot, int port, struct k1k2 * k1k2),
               void (*doswitch)(int ringid, int slot[NUM_SIDES], int port[NUM_SIDES],
                                enum node_state oldstate, enum node_state curstate),
               void (*startwtr)(int ringid, int enable, int sec)) {
    assert(aps);
    if (doswitch) {
        if (aps->node_state != aps->node_state_filter) {
            doswitch(aps->ring_id, aps->slot, aps->port, aps->node_state_filter, aps->node_state);
            aps->node_state_filter = aps->node_state;
        }
    }
    if (sendkbyte) {
        if (memcmp(&aps->cur_kbytes[WEST], &aps->cur_kbytes_filter[WEST], sizeof(aps->cur_kbytes[WEST]))) {
            sendkbyte(aps->ring_id, aps->slot[WEST], aps->port[WEST], &aps->cur_kbytes[WEST]);
            memcpy(&aps->cur_kbytes[WEST], &aps->cur_kbytes_filter[WEST], sizeof(aps->cur_kbytes[WEST]));
        }
        if (memcmp(&aps->cur_kbytes[EAST], &aps->cur_kbytes_filter[EAST], sizeof(aps->cur_kbytes[EAST]))) {
            sendkbyte(aps->ring_id, aps->slot[EAST], aps->port[EAST], &aps->cur_kbytes[EAST]);
            memcpy(&aps->cur_kbytes[EAST], &aps->cur_kbytes_filter[EAST], sizeof(aps->cur_kbytes[EAST]));
        }
    }
    if (startwtr) {
        if (aps->is_wtr_start != aps->is_wtr_start_filter) {
            startwtr(aps->ring_id, aps->is_wtr_start, aps->wtr_time);
            aps->is_wtr_start_filter = aps->is_wtr_start;
        }
        
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
