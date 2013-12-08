#include "ring_def.h"
#include "ring_state.h"
#include "ring_state_switch.h"
#include "ring_state_wtr.h"
#include "ring_state_primary.h"
#include "ring_aps_controller.h"
#include "ring_rule_s.h"
#include "ring_funcs.h"
#include "ring_state_fs.h"
#include "ring_dbg.h"

#include <assert.h>

void fs_start_up_change(struct aps_controller* aps)
{
    start_up_change(aps);
}

void fs_idle_and_waiting_change(struct aps_controller* aps)
{
    idle_and_waiting_change(aps);
}

void fs_br_and_waiting_change(struct aps_controller* aps)
{
	br_and_waiting_change(aps);
}

void fs_brsw_change(struct aps_controller* aps)
{
	brsw_change(aps);
}

void fs_state_exit(struct aps_controller* aps)
{
    aps->cur_end_state = END_START_UP;
}

void fs_state_run(struct aps_controller* aps)
{
    assert(aps);

	switch(aps->cur_end_state)
	{
	case END_START_UP:
	    fs_start_up_change(aps);
		break;
	case END_IDLE_AND_WAITING:
		fs_idle_and_waiting_change(aps);
	    break;
	case END_BR_AND_WAITING:
		fs_br_and_waiting_change(aps);
		break;
	case END_BRSW:
	    fs_brsw_change(aps);
		break;
	default:
		assert(0);
		break;
	}
	//ring_print(aps->node_id, "   exit fs, state = %s\n", end_state_name(aps->cur_end_state));
}
