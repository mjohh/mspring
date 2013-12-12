mspring
=======

g.841 mspring impl except span protection

current states
unit test has been done to verfify the aps state machine

api usage
1) config a ring in one node:

// define an aps controller obj
struct aps_controller g_aps;

// init the obj
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

2) aps input
// set ne ready when node start up
aps_input_ne_ready_flag(&g_aps, 1);
