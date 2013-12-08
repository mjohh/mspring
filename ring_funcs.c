#include "ring_def.h"
#include "ring_state.h"
#include "ring_state_switch.h"
#include "ring_state_wtr.h"
#include "ring_state_primary.h"
#include "ring_aps_controller.h"
#include "ring_funcs.h"

#include <memory.h>
#include <assert.h>

//// this file impl rule s-s
enum brq_code get_highest_brq_on_side(struct aps_controller *aps, enum side side)
{
    int i;
    enum brq_code brqs[3];
    enum brq_code hi_brq = NR;
    
    brqs[0] = get_dq_brq(aps, side);
    brqs[1] = side == EXT_CMD_SIDE ? get_ext_brq(aps) : NR;
    brqs[2] = get_kbyte_brq(aps, side);
    
    for (i = 0; i < 3; i++) {
        if (brqs[i] > hi_brq) {
            hi_brq = brqs[i];
        }
    }
    return hi_brq;
}

enum brq_code get_highest_brq(struct aps_controller *aps)
{
    return RMAX(get_highest_brq_on_side(aps, WEST), get_highest_brq_on_side(aps, EAST));
}

enum brq_code get_local_brq(struct aps_controller *aps, enum side side)
{
    assert(WEST == EXT_CMD_SIDE || EAST == EXT_CMD_SIDE);
    return EXT_CMD_SIDE == side ? RMAX(get_ext_brq(aps), get_dq_brq(aps, side)) : get_dq_brq(aps, side);
}

int is_coexist_brq(enum brq_code wbrq, enum brq_code ebrq)
{
    return wbrq == ebrq || (wbrq == LOP && ebrq == LOP) || ((wbrq == SF || wbrq == FS) && (ebrq == SF || ebrq == FS)) || ((wbrq == SD || wbrq == MS || wbrq == EXER) && (ebrq == SD || ebrq == MS || ebrq == EXER));
}

enum side get_side_by_slot_port(struct aps_controller *aps, int slot, int port)
{
    assert(aps);
    if (aps->slot[WEST] == slot && aps->port[WEST] == port) {
        return WEST;
    } else if (aps->slot[EAST] == slot && aps->port[EAST] == port) {
        return EAST;
    } else {
        assert(0);
    }
    return NUM_SIDES;
}

// TODO :
int is_leagal_kbytes(struct aps_controller *aps, struct k1k2 *k1k2)
{
    if (!is_default_kbytes(k1k2) && k1k2->k1.brcode != NR) {
        if (k1k2->k2.path == SHORT_PATH && k1k2->k1.dest_node != aps->node_id)
            return 0;
    }
    return 1;
}

void update_node_state(struct aps_controller *aps, enum node_state state)
{
    assert(aps && IDLE <= state && state < ILLEGAL_STATE);
    aps->node_state = state;
}

enum brq_code get_kbyte_brq(struct aps_controller *aps, enum side side)
{
    if (is_cut(aps, side)) {
        return NR;
    }
    return DRV_KBYTES_BRCODE(side);
}

enum brq_code get_dq_brq(struct aps_controller *aps, enum side side)
{
    if (DQ_SF == DRV_DQ(side) || is_cut(aps, side)) {
        return SF;
    }
    if (DQ_SD == DRV_DQ(side)) {
        return SD;
    }
    return NR;
}

// attention:
// for ext cmd, here just handle EX_LOP.
// for EX_LOW, it do not send brq code.
enum brq_code get_ext_brq(struct aps_controller *aps)
{
    switch (EXT_CMD_CMD) {
        case EX_LOP:
            return LOP;
        case EX_FS:
            return FS;
        case EX_MAN:
            return MS;
        case EX_EXER:
            return EXER;
        default:
            //if (DRV_EXERCISE_SIDE != UNDEFINED_SIDE)
            //  {
            //      return EXER;
            //  }
            return NR;
    }
}

enum brq_code get_highest_brq_for_me(struct aps_controller *aps)
{
    int i;
    enum brq_code brqs[NUM_BRQ_SOURCES];
    enum brq_code hi_brq = NR;
    
    brqs[0] = get_dq_brq(aps, WEST);
    brqs[1] = get_dq_brq(aps, EAST);
    brqs[2] = get_ext_brq(aps);
    brqs[3] = get_kbyte_brq_for_me(aps, WEST);
    brqs[4] = get_kbyte_brq_for_me(aps, EAST);
    
    for (i = 0; i < NUM_BRQ_SOURCES; i++) {
        if (brqs[i] > hi_brq) {
            hi_brq = brqs[i];
        }
    }
    return hi_brq;
}

enum brq_code get_kbyte_brq_for_me(struct aps_controller *aps, enum side side)
{
    if (!is_kbyte_for_me(aps, side)) {
        return NR;
    }
    if (is_cut(aps, side)) {
        return NR;
    }
    return DRV_KBYTES_BRCODE(side);
}

int is_kbyte_for_me(struct aps_controller *aps, enum side side)
{
    return (MY_NODE_ID == DRV_KBYTES_DEST(side));
}

enum brq_code get_kbyte_brq_for_other(struct aps_controller *aps, enum side side)
{
    if (is_kbyte_for_me(aps, side)) {
        return NR;
    }
    return get_kbyte_brq(aps, side);
}

enum brq_code get_highest_brq_for_other(struct aps_controller *aps)
{
    enum brq_code west_brq = NR;
    enum brq_code east_brq = NR;
    
    /* if both k bytes are addressed to me, then brq type for other is nill */
    if (is_kbyte_for_me(aps, WEST) && is_kbyte_for_me(aps, EAST)) {
        return NR;
    }
    if (!is_kbyte_for_me(aps, WEST)) {
        west_brq = get_kbyte_brq(aps, WEST);
    }
    if (!is_kbyte_for_me(aps, EAST)) {
        east_brq = get_kbyte_brq(aps, EAST);
    }
    if (west_brq > east_brq)
        return west_brq;
    return east_brq;
}

int is_default_kbytes(struct k1k2 *kbyte)
{
    char k1 = DEFAULT_K1BYTE;
    char k2 = DEFAULT_K2BYTE;
    return (0 == memcmp(&kbyte->k1, &k1, sizeof(k1)) && 0 == memcmp(&kbyte->k2, &k2, sizeof(k2)));
}

int is_cut(struct aps_controller *aps, enum side side)
{
    return !are_kbytes_readable(aps, side);
}

int are_kbytes_readable(struct aps_controller *aps, enum side side)
{
    /* before deciding check status of kbytes, maybe not yet available */
    if (DQ_SF == DRV_DQ(side))
        return 0;
    if (MS_AIS == DRV_KBYTES_STATUS(side))
        return 0;
    if (!IS_NE_READY)
        return 0;
    if (is_default_kbytes(&DRV_KBYTES(side)))
        return 0;
    return 1;
}

enum brq_code get_brq_for_me(struct aps_controller *aps, enum side side)
{
    int i;
    enum brq_code brqs[3];
    enum brq_code hi_brq = NR;
    
    brqs[0] = get_dq_brq(aps, side);
    brqs[1] = side == EXT_CMD_SIDE ? get_ext_brq(aps) : NR;
    brqs[2] = get_kbyte_brq_for_me(aps, side);
    
    for (i = 0; i < 3; i++) {
        if (brqs[i] > hi_brq) {
            hi_brq = brqs[i];
        }
    }
    return hi_brq;
}

enum brq_code get_brq_for_other(struct aps_controller *aps, enum side side)
{
    /* if both k bytes are addressed to me, then brq type for other is nill */
    if (is_kbyte_for_me(aps, side)) {
        return NR;
    }
    return get_kbyte_brq(aps, side);
}

enum brq_code get_short_brq(struct aps_controller *aps, enum side side)
{
    enum brq_code brq = get_kbyte_brq_for_me(aps, side);
    
    if (LONG_PATH == DRV_KBYTES_PATH(side)) {
        return NR;
    }
    
    if (NR == brq || RR == brq) {
        return NR;
    }
    return brq;
}

enum brq_code get_long_brq(struct aps_controller *aps, enum side side)
{
    enum brq_code brq = get_kbyte_brq(aps, side);
    
    if (SHORT_PATH == DRV_KBYTES_PATH(side)) {
        return NR;
    }
    
    if (NR == brq || RR == brq) {
        return NR;
    }
    return brq;
}

char *get_name_of_brq(enum brq_code brq)
{
    switch (brq) {
        case NR:
            return "NR";
        case RR:
            return "RR";
        case RR_S:
            return "RR_S";
        case EXER:
            return "EXER";
        case EXER_S:
            return "EXER_S";
        case WTR:
            return "WTR";
        case MS:
            return "MS";
        case MS_S:
            return "MS_S";
        case SD:
            return "SD";
        case SD_S:
            return "SD_S";
        case SD_P:
            return "SD_P";
        case SF:
            return "SF";
        case SF_S:
            return "SF_S";
        case FS:
            return "FS";
        case FS_S:
            return "FS_S";
        case LOP:
            return "LOP";
        default:
            return "err br code!!!";
    }
}

char *get_name_of_status(enum status stat)
{
    switch (stat) {
        case IDLE_STATUS:
            return "IDLE";
        case BR_STATUS:
            return "BR";
        case BRSW_STATUS:
            return "BRSW";
        case ET:
            return "ET";
        case IDLE_ACK_STATUS:
            return "ICK";
        case NOT_IDLE_ACK_STATUS:
            return "NICK";
        case RDI_STATUS:
            return "RDI";
        case MS_AIS:
            return "MS_AIS";
        default:
            return "err status!!!";
    }
}

char *dq_name(enum dq dq)
{
    switch (dq) {
        case DQ_CLEAR:
            return "DQ_CLEAR";
        case DQ_SD:
            return "DQ_SD";
        case DQ_SF:
            return "DQ_SF";
        case DQ_K1K2_INCONSISTENCY:
            return "DQ_K1K2_INCONSISTENCY";
        case NUM_DQ_CODES:
            return "NUM_DQ_CODES";
        default:
            return "err dq!!!";
    }
}

int is_valid_long_reply_for_brq(struct aps_controller *aps, enum brq_code brq, enum side reply_side)
{
    if (LONG_PATH == DRV_KBYTES_PATH(reply_side)) {
        if (is_coexist_brq(DRV_KBYTES_BRCODE(reply_side), brq)) {
            return 1;
        }
        
        if (MY_NODE_ID == DRV_KBYTES_DEST(reply_side)) {
            return 1;
        }
    }
    return 0;
}

int is_tail_end(struct aps_controller *aps)
{
    enum brq_code short_brq = get_kbyte_brq_for_me(aps, aps->short_side);
    enum brq_code local_brq = get_local_brq(aps, aps->short_side);
    enum brq_code hi_brq = get_highest_brq(aps);
    return (RR == short_brq || (NR != local_brq && local_brq == hi_brq));
}

// attention: one short nr brq my not to the node, see tail-end wtr timeout process.
int is_rcv_nr_idle_both_sides(struct aps_controller *aps)
{
    enum brq_code w_brq = get_kbyte_brq(aps, WEST);
    enum brq_code e_brq = get_kbyte_brq(aps, EAST);
    
    return (NR == w_brq && NR == e_brq && SHORT_PATH == DRV_KBYTES_PATH(WEST) && SHORT_PATH == DRV_KBYTES_PATH(EAST));
}
