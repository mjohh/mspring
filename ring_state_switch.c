#include "ring_def.h"
#include "ring_state.h"
#include "ring_state_switch.h"
#include "ring_state_wtr.h"
#include "ring_state_primary.h"
#include "ring_aps_controller.h"
#include "ring_funcs.h"
#include "ring_state_sd.h"
#include "ring_state_ms.h"
#include "ring_state_sf.h"
#include "ring_state_fs.h"
#include "ring_state_exer.h"
#include "ring_state_lp.h"
#include "ring_rule_s.h"
#include "ring_state_sw_upgrd.h"
#include "ring_dbg.h"

#include <assert.h>

static void set_state(struct aps_controller *aps, enum switch_state_id next_state)
{
    assert(aps && next_state >= SW_STARTUP && next_state < SW_STATE_MAX);
    aps->cur_sw_state = next_state;
}

static void to_new_state(struct aps_controller *aps)
{
    enum brq_code brq;
    enum side short_side;
    get_reason_brq_and_short_side(aps, &brq, &short_side);;
    
    switch (brq) {
        case EXER:
            set_state(aps, SW_EXER);
            exer_state_run(aps);
            break;
        case MS:
            set_state(aps, SW_MS);
            ms_state_run(aps);
            break;
        case SD:
            set_state(aps, SW_SD);
            sd_state_run(aps);
            break;
        case SF:
            set_state(aps, SW_SF);
            sf_state_run(aps);
            break;
        case FS:
            set_state(aps, SW_FS);
            fs_state_run(aps);
            break;
        case LOP:
            set_state(aps, SW_LP);
            lp_state_run(aps);
            break;
        default:
            //e_assert(0);
            //idle switching node recv two long form it's neib, do nothing.
            break;
    }
}


///////////////////////////////////////////////////////////////////////////////

char *sw_state_name(int state)
{
    switch (state) {
        case SW_STARTUP:
            return "SW_STARTUP";
        case SW_EXER:
            return "SW_EXER";
        case SW_MS:
            return "SW_MS";
        case SW_SD:
            return "SW_SD";
        case SW_SF:
            return "SW_SF";
        case SW_FS:
            return "SW_FS";
        case SW_LP:
            return "SW_LP";
        default:
            return "err sw state";
    }
}

void sw_state_run(struct aps_controller *aps)
{
    assert(aps);
    
    switch (aps->cur_sw_state) {
        case SW_STARTUP:
        case SW_EXER:
        case SW_MS:
        case SW_SD:
        case SW_SF:
        case SW_FS:
        case SW_LP:
            to_new_state(aps);
            break;
        default:
            assert(0);
            break;
    }
    //ring_print(aps->node_id, "  exit sw, state = %sn", sw_state_name(aps->cur_sw_state));
}

void sw_state_exit(struct aps_controller *aps)
{
    aps->cur_sw_state = SW_STARTUP;
}

void sw_state_init(struct aps_controller *aps)
{
    assert(aps);
    aps->cur_sw_state = SW_STARTUP;
    ring_print(aps->node_id, "init sw, state = %sn", sw_state_name(aps->cur_sw_state));
}

void sw_state_fini(struct aps_controller *aps)
{
    // do nothing
}
