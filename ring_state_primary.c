#include "ring_def.h"
#include "ring_state.h"
#include "ring_state_switch.h"
#include "ring_state_wtr.h"
#include "ring_state_primary.h"
#include "ring_aps_controller.h"
#include "ring_funcs.h"
#include "ring_state.h"
#include "ring_state_sw_upgrd.h"
#include "ring_dbg.h"

#include <memory.h>
#include <assert.h>

static void set_state(struct aps_controller *aps, enum primary_state_id next_state);
void fill_nr_idle(struct aps_controller *aps);


int is_to_switch(struct aps_controller *aps)
{
    enum brq_code w_brq = get_brq_for_me(aps, WEST);
    enum brq_code e_brq = get_brq_for_me(aps, EAST);
    enum brq_code hi_brq = get_highest_brq(aps);
    
    if (RMAX(w_brq, e_brq) != NR && RMAX(w_brq, e_brq) != RR && (is_coexist_brq(RMAX(w_brq, e_brq), hi_brq))) {
        return 1;
    }
    return 0;
}

// rule P#2
int is_recv_same_pri_long_brq_to_me(struct aps_controller *aps)
{
    enum brq_code w_hi_brq = get_highest_brq_on_side(aps, WEST);
    enum brq_code e_hi_brq = get_highest_brq_on_side(aps, EAST);
    if (w_hi_brq == e_hi_brq && w_hi_brq > NR) {
        if (LONG_PATH == DRV_KBYTES_PATH(EAST) && LONG_PATH == DRV_KBYTES_PATH(WEST)) {
            if (is_kbyte_for_me(aps, WEST) && !is_kbyte_for_me(aps, EAST))
                return 1;
            if (is_kbyte_for_me(aps, EAST) && !is_kbyte_for_me(aps, WEST))
                return 1;
        }
    }
    return 0;
}

// for node failure recover, state after rule P#2
int is_recv_long_brq_and_nr_from_same_neib(struct aps_controller *aps)
{
    enum brq_code w_hi_brq = get_highest_brq_on_side(aps, WEST);
    enum brq_code e_hi_brq = get_highest_brq_on_side(aps, EAST);
    if (w_hi_brq > NR && NR == e_hi_brq) {
        if (LONG_PATH == DRV_KBYTES_PATH(WEST) && SHORT_PATH == DRV_KBYTES_PATH(EAST)) {
            if (DRV_KBYTES_SRC(WEST) == DRV_KBYTES_SRC(EAST)) {
                return 1;
            }
        }
    }
    if (e_hi_brq > NR && NR == w_hi_brq) {
        if (LONG_PATH == DRV_KBYTES_PATH(EAST) && SHORT_PATH == DRV_KBYTES_PATH(WEST)) {
            if (DRV_KBYTES_SRC(EAST) == DRV_KBYTES_SRC(WEST)) {
                return 1;
            }
        }
    }
    return 0;
}

int is_to_pass(struct aps_controller *aps)
{
    enum brq_code w_brq = get_brq_for_me(aps, WEST);
    enum brq_code e_brq = get_brq_for_me(aps, EAST);
    enum brq_code hi_brq = get_highest_brq(aps);
    
    if (NR != RMAX(w_brq, e_brq) && is_coexist_brq(RMAX(w_brq, e_brq), hi_brq))
        return 0;
    
    if (RMAX(w_brq, e_brq) < hi_brq)
        return 1;
    
    if (NR == RMAX(w_brq, e_brq)) {
        if (!is_default_kbytes(&DRV_KBYTES(WEST)) && !is_kbyte_for_me(aps, WEST)) {
            return 1;
        }
        if (!is_default_kbytes(&DRV_KBYTES(EAST)) && !is_kbyte_for_me(aps, EAST)) {
            return 1;
        }
    }
    return 0;
}


int is_to_k_pass(struct aps_controller *aps)
{
    return is_to_pass(aps) && get_highest_brq(aps) == EXER;
}

int is_to_idle(struct aps_controller *aps)
{
    return NR == get_highest_brq(aps);
}

int is_to_wtr(struct aps_controller *aps)
{
    enum brq_code short_brq = NR;
    int is_for_me = 0;
    int is_long = 0;
    enum side new_side;
    enum brq_code new_brq;
    
    if (NUM_SIDES == aps->short_side) {
        return 0;
    }
    
    get_reason_brq_and_short_side(aps, &new_brq, &new_side);
    if (new_side != aps->short_side) {
        return 0;
    }
    
    short_brq = get_kbyte_brq_for_me(aps, aps->short_side);
    is_for_me = (DRV_KBYTES_DEST(OTHER_SIDE(aps->short_side)) == MY_NODE_ID);
    is_long = (DRV_KBYTES_PATH(OTHER_SIDE(aps->short_side)) == LONG_PATH);
    
    // tail end
    if (RR == short_brq /*&& is_long && is_for_me */ ) {
        if (NR == get_dq_brq(aps, WEST) && NR == get_dq_brq(aps, EAST)) {
            //if(get_ext_brq(aps) <= EXER)
            if (NR == get_ext_brq(aps)) {
                return 1;
            }
        }
    }
    // head end or tail end cleared
    if ((WTR == short_brq || NR == short_brq) /*&& is_long && is_for_me */ ) {
        if (NR == get_dq_brq(aps, WEST) && NR == get_dq_brq(aps, EAST)) {
            //if(get_ext_brq(aps) <= EXER)
            if (NR == get_ext_brq(aps)) {
                return 1;
            }
        }
    }
    return 0;
}

int is_only_rcv_brq_it_sourcing_both_sides(struct aps_controller *aps)
{
    enum brq_code w_hi_brq = get_highest_brq_on_side(aps, WEST);
    enum brq_code e_hi_brq = get_highest_brq_on_side(aps, EAST);
    enum brq_code w_kbyte_brq = get_kbyte_brq(aps, WEST);
    enum brq_code e_kbyte_brq = get_kbyte_brq(aps, EAST);
    
    return (w_hi_brq == w_kbyte_brq && e_hi_brq == e_kbyte_brq && DRV_KBYTES_SRC(WEST) == MY_NODE_ID && DRV_KBYTES_SRC(EAST) == MY_NODE_ID);
}

void tx_default_kbytes(struct aps_controller *aps)
{
    char k1 = DEFAULT_K1BYTE;
    char k2 = DEFAULT_K2BYTE;
    memcpy(&CURRENT_KBYTES_K1(WEST), &k1, sizeof(k1));
    memcpy(&CURRENT_KBYTES_K2(WEST), &k2, sizeof(k2));
    memcpy(&CURRENT_KBYTES_K1(EAST), &k1, sizeof(k1));
    memcpy(&CURRENT_KBYTES_K2(EAST), &k2, sizeof(k2));
}

static void start_up_state_change(struct aps_controller *aps)
{
    if (!IS_NE_READY) {
        tx_default_kbytes(aps);
        return;
    }
    if (is_to_idle(aps)) {
        fill_nr_idle(aps);
        set_state(aps, PRIM_IDLE);
        update_node_state(aps, IDLE);
    } else if (is_to_switch(aps)) {
        drop_switch_if_occupy(aps);
        set_state(aps, PRIM_SWITCH);
        sw_state_run(aps);	// go to next layer
    } else if (is_to_pass(aps)) {
        set_state(aps, PRIM_PASS);
        update_node_state(aps, PASS);
    } else if (is_to_k_pass(aps)) {
        set_state(aps, PRIM_K_PASS);
        update_node_state(aps, k_PASS);
    } else {
        assert(0);
    }
}


///////////////////////////////////////////////////////////////////////////////
static void idle_state_change(struct aps_controller *aps)
{
    // keep idle state
    if (is_to_idle(aps)) {
        fill_nr_idle(aps);
        set_state(aps, PRIM_IDLE);
        update_node_state(aps, IDLE);
    } else if (is_to_switch(aps)) {
        drop_switch_if_occupy(aps);
        set_state(aps, PRIM_SWITCH);
        sw_state_run(aps);	// go to next layer
    } else if (is_to_pass(aps)) {
        set_state(aps, PRIM_PASS);
        update_node_state(aps, PASS);
    } else if (is_to_k_pass(aps)) {
        set_state(aps, PRIM_K_PASS);
        update_node_state(aps, k_PASS);
    } else {
        assert(0);
    }
}


///////////////////////////////////////////////////////////////////////////////
static void switch_state_change(struct aps_controller *aps)
{
    // to idle state
    if (is_only_rcv_brq_it_sourcing_both_sides(aps)) {
        fill_nr_idle(aps);
        exit_cur_sw_state(aps);	// exit switch and it's sub states
        set_state(aps, PRIM_IDLE);
        update_node_state(aps, IDLE);
    } else if (is_to_pass(aps)) {
        exit_cur_sw_state(aps);	// exit switch and it's sub states
        set_state(aps, PRIM_PASS);
        update_node_state(aps, PASS);
    } else if (is_to_wtr(aps)) {
        // note: here need not exit switch and it's sub states
        // because switch state should be reserved for returning from wtr some time.
        // goto next layer of wtr sm
        set_state(aps, PRIM_WTR);
        wtr_state_run(aps);
    } else if (is_to_switch(aps)) {
        drop_switch_if_occupy(aps);
        set_state(aps, PRIM_SWITCH);
        sw_state_run(aps);	// go to next layer
    } else if (is_to_k_pass(aps)) {
        exit_cur_sw_state(aps);	// exit switch and it's sub states
        set_state(aps, PRIM_K_PASS);
        update_node_state(aps, k_PASS);
    } else {
        assert(0);
    }
}


///////////////////////////////////////////////////////////////////////////////
static void pass_state_change(struct aps_controller *aps)
{
    // to idle state
    if (NR == get_highest_brq_for_me(aps) && is_rcv_nr_idle_both_sides(aps)) {
        fill_nr_idle(aps);
        set_state(aps, PRIM_IDLE);
        update_node_state(aps, IDLE);
    }
    // keep pass state
    else if (is_recv_same_pri_long_brq_to_me(aps) || is_recv_long_brq_and_nr_from_same_neib(aps) || is_to_pass(aps)) {
        set_state(aps, PRIM_PASS);
        update_node_state(aps, PASS);
    } else if (is_to_switch(aps)) {
        drop_switch_if_occupy(aps);
        set_state(aps, PRIM_SWITCH);
        sw_state_run(aps);	// go to next layer
    } else if (is_to_k_pass(aps)) {
        set_state(aps, PRIM_K_PASS);
        update_node_state(aps, k_PASS);
    } else {
        assert(0);
    }
}


///////////////////////////////////////////////////////////////////////////////
static void k_pass_state_change(struct aps_controller *aps)
{
    // to idle state
    if (is_rcv_nr_idle_both_sides(aps)) {
        fill_nr_idle(aps);
        set_state(aps, PRIM_IDLE);
        update_node_state(aps, IDLE);
    } else if (is_to_switch(aps)) {
        drop_switch_if_occupy(aps);
        set_state(aps, PRIM_SWITCH);
        sw_state_run(aps);	// go to next layer
    } else if (is_to_pass(aps)) {
        set_state(aps, PRIM_PASS);
        update_node_state(aps, PASS);
    } else {
        assert(0);
    }
}


///////////////////////////////////////////////////////////////////////////////
static void wtr_state_change(struct aps_controller *aps)
{
    // to idle state
    // in the last version, wtr to idle occurs in wtr sub state machine
    // but now i remove it here
    if (is_rcv_nr_idle_both_sides(aps)) {
        fill_nr_idle(aps);
        exit_cur_sw_state(aps);
        wtr_state_exit(aps);
        set_state(aps, PRIM_IDLE);
        update_node_state(aps, IDLE);
    } else if (is_to_pass(aps)) {
        exit_cur_sw_state(aps);
        wtr_state_exit(aps);
        set_state(aps, PRIM_PASS);
        update_node_state(aps, PASS);
    }
    // keep in wtr process
    else if (is_to_wtr(aps)) {
        set_state(aps, PRIM_WTR);
        wtr_state_run(aps);
    } else if (is_to_switch(aps)) {
        drop_switch_if_occupy(aps);
        set_state(aps, PRIM_SWITCH);
        wtr_state_exit(aps);	//exit wtr state firstly
        sw_state_run(aps);	// go to switch state
    } else if (is_to_k_pass(aps)) {
        exit_cur_sw_state(aps);
        wtr_state_exit(aps);
        set_state(aps, PRIM_K_PASS);
        update_node_state(aps, k_PASS);
    } else {
        assert(0);
    }
}

///////////////////////////////////////////////////////////////////////////////


static void set_state(struct aps_controller *aps, enum primary_state_id next_state)
{
    assert(aps && next_state >= PRIM_START_UP && next_state < PRIM_STATE_MAX);
    aps->cur_prim_state = next_state;
}

char *prim_state_name(int state)
{
    switch (state) {
        case PRIM_START_UP:
            return "PRIM_START_UP";
        case PRIM_IDLE:
            return "PRIM_IDLE";
        case PRIM_SWITCH:
            return "PRIM_SWITCH";
        case PRIM_WTR:
            return "PRIM_WTR";
        case PRIM_PASS:
            return "PRIM_PASS";
        case PRIM_K_PASS:
            return "PRIM_K_PASS";
        default:
            return "err prim state";
    }
}


void prim_state_init(struct aps_controller *aps)
{
    tx_default_kbytes(aps);
    aps->cur_prim_state = PRIM_START_UP;
    ring_print(aps->node_id, "init prim, state = %sn", prim_state_name(aps->cur_prim_state));
}

void prim_state_run(struct aps_controller *aps)
{
    assert(aps);
    
    switch (aps->cur_prim_state) {
        case PRIM_START_UP:
            start_up_state_change(aps);
            break;
        case PRIM_IDLE:
            idle_state_change(aps);
            break;
        case PRIM_SWITCH:
            switch_state_change(aps);
            break;
        case PRIM_WTR:
            wtr_state_change(aps);
            break;
        case PRIM_PASS:
            pass_state_change(aps);
            break;
        case PRIM_K_PASS:
            k_pass_state_change(aps);
            break;
        default:
            assert(0);
            break;
    }
    if (aps->output_update_hw) {
        aps->output_update_hw(aps->ring_id, aps->slot, aps->port, aps->node_state);
    }
    if (aps->output_set_kbyte) {
        aps->output_set_kbyte(aps->ring_id, aps->slot[WEST], aps->port[WEST], &aps->cur_kbytes[WEST]);
        aps->output_set_kbyte(aps->ring_id, aps->slot[EAST], aps->port[EAST], &aps->cur_kbytes[EAST]);
    }
    if (aps->output_start_wtr) {
        aps->output_start_wtr(aps->ring_id, aps->is_wtr_start, aps->wtr_time);
    }
    //ring_print(aps->node_id, "exit prim, state = %sn", prim_state_name(aps->cur_prim_state));
}

void prim_state_exit(struct aps_controller *aps)
{
    aps->cur_prim_state = PRIM_START_UP;
}

void prim_state_fini(struct aps_controller *aps)
{
    // do nothing
}

void fill_nr_idle(struct aps_controller *aps)
{
    UPDATE_CURRENT_KBYTES(WEST, NR, NEIB_NODE_ID(WEST), MY_NODE_ID, SHORT_PATH, IDLE_STATUS);
    UPDATE_CURRENT_KBYTES(EAST, NR, NEIB_NODE_ID(EAST), MY_NODE_ID, SHORT_PATH, IDLE_STATUS);
}
