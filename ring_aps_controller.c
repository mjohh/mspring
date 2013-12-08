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

int aps_controller_init(struct aps_controller* aps,
                        int (*set_kbyte)(int ringid, int cardid, int portid, struct k1k2 * k1k2),
                        int (*update_hw)(int ringid, int cardid[], int portid[], enum node_state state),
                        int (*start_wtr)(int ringid, int enable, int sec))
{
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
    
    aps->set_kbyte = set_kbyte;
    aps->update_hw = update_hw;
    aps->start_wtr = start_wtr;
    return 0;
}

void aps_controller_fini(struct aps_controller* aps)
{
    assert(aps);
	prim_state_fini(aps);
    wtr_state_fini(aps);
    sw_state_fini(aps);
	end_state_fini(aps);
}

void aps_controller_run(struct aps_controller* aps)
{
    assert(aps);
    prim_state_run(aps);
}

void aps_update_kbyte(struct aps_controller * aps, enum side side, struct k1k2 * k1k2)
{
    assert(aps && k1k2);
    assert(WEST == side || EAST == side);
    memcpy(&aps->drv_kbytes[side], k1k2, sizeof(*k1k2));
}
void aps_update_dq(struct aps_controller * aps, enum side side, enum dq dq)
{
    assert(aps);
    assert(WEST == side || EAST == side);
    assert(dq >= DQ_CLEAR && dq < NUM_DQ_CODES);
    aps->dq[side] = dq;
}
void aps_update_ext_cmd(struct aps_controller * aps, enum side side, enum ext_cmd ext_cmd)
{
    assert(aps);
    assert(WEST == side || EAST == side);
    assert(ext_cmd >= EX_NONE && ext_cmd < NUM_EXT_CMDS);
    aps->ext_cmd = ext_cmd;
    aps->ext_side = side;
}
void aps_update_ne_ready_flag(struct aps_controller * aps, int is_ne_ready)
{
    assert(aps);
    aps->is_ne_ready = is_ne_ready;
}
void aps_update_wtr_timeout_flag(struct aps_controller * aps, int is_wtr_timeout)
{
    assert(aps);
    aps->is_wtr_timeout = is_wtr_timeout;
}


