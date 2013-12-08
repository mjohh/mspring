#include "ring_def.h"
#include "ring_state.h"
#include "ring_state_switch.h"
#include "ring_state_wtr.h"
#include "ring_state_primary.h"
#include "ring_aps_controller.h"
#include "ring_funcs.h"


extern void switching_state_test_run(void);
extern void switch_to_switch_test_run(void);
extern void passthru_test_run(void);
extern void node_start_up_test_run(void);
extern void node_start_up_neib_test_run(void);
extern void swtich_idle_transition_test_run(void);

void unit_test(void)
{
	node_start_up_test_run();
	node_start_up_neib_test_run();
    switching_state_test_run();
	swtich_idle_transition_test_run();
	switch_to_switch_test_run();
	passthru_test_run();
}