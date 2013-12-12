# G.841 mspring implementation(not include span switch)

# Current status

Unit test has been done to verfify the aps state machine

## API Usage:

Recommended head file includes:

    #include "ring_def.h"
    #include "ring_state.h"
    #include "ring_state_switch.h"
    #include "ring_state_wtr.h"
    #include "ring_state_primary.h"
    #include "ring_aps_controller.h"

Initialize, config a ring:

    // define an aps controller
    struct aps_controller g_aps;
    // init the aps
    int ret = aps_controller_init(&g_aps);
    assert(0 == ret);
    // cfg the ring
    unsigned short ring_map[] = {0,1,2,3};
    int nodes_num = sizeof(ring_map) / sizeof(ring_map[0]);
    int node_id = 2;
    int ring_id = 1;
    int slot[] = {2, 3};
    int port[] = {1, 2};
    int wtr_time = 10;//second
    ret = aps_cfg(&g_aps, ring_map, nodes_num, node_id, ring_id, slot, port, wtr_time);
    assert(0 == ret);

Input aps conditions

    // node start up
    aps_input_ne_ready_flag(&g_aps, 1);
    // west sf
    aps_input_dq(&g_aps, WEST, DQ_SF);
    // east force switch commond
    aps_input_ext_cmd(&g_aps, EAST, EX_FS);
    // west k bytes
    struc k1k2 kbyte;
    UPDAE_KBYTES(kbyte, WEST, SF, 2, 1, SHORT_PATH, BRSW_STATUS);
    aps_input_kbyte(&g_aps, &kbyte);
    // wtr expiration
    aps_input_wtr_timeout(&g_aps, 1);
