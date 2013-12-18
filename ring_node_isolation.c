#include "ring_def.h"
#include "ring_state.h"
#include "ring_state_switch.h"
#include "ring_state_wtr.h"
#include "ring_state_primary.h"
#include "ring_aps_controller.h"
#include "ring_funcs.h"
#include <assert.h>

// only sf and fs need squelch
int is_interface_unavailabe(struct aps_controller* aps, enume side side) {
    assert(aps && (WEST == side || EAST == side));
    if (is_cut(aps, side) || (EXT_CMD_CMD == EX_FS && EXT_CMD_SIDE == side))// tail
        return 1;
    if (get_short_brq(aps, side) == SF || get_short_brq(aps, side) == FS)// head
        return 1;
    return 0;
}

int is_recv_brq_from_nonneighbour_on_long_side(struct aps_controller* aps, enume longside) {
    enum side shortside = (longside == WEST ? EAST : WEST);
    assert(aps && (WEST == side || EAST == longside));
    if ((DRV_KBYTES_BRCODE(longside) == SF || DRV_KBYTES_BRCODE(longside) == FS) &&
        DRV_KBYTES_SRC(longside) != NEIB_NODE_ID(shortside)
        return 1;
    return 0;
}

int is_recv_brq_from_nonneighbours_both_sides(struct aps_controller* aps) {
    assert(aps);
    if ((DRV_KBYTES_BRCODE(WEST) == SF || DRV_KBYTES_BRCODE(WEST) == FS) &&
        (DRV_KBYTES_BRCODE(EAST) == SF || DRV_KBYTES_BRCODE(EAST) == FS) &&
        DRV_KBYTES_SRC(WEST) != NEIB_NODE_ID(EAST) &&
        DRV_KBYTES_SRC(EAST) != NEIB_NODE_ID(WEST) &&
        DRV_KBYTES_SRC(WEST) != DRV_KBYTES_SRC(EAST))
        return 1;
    return 0;
}

// node is isolated if both of its interfaces are unavailable.
// node is isolated if one of its interface is unavailable and
// from the other side it recieves KBytes not from his neighbor.
// node is isolated if it recieves BRQ (FS,SF) from both side
// from nodes that are not neighbors.
eint is_isolated(struct aps_controller* aps) {
  int west_unavailable = is_interface_unavailabe(aps, WEST);
  int east_unavailable = is_interface_unavailabe(aps, EAST);
  if (east_unavailable && east_unavailable)
     return 1;
  if (east_unavailable && is_recv_brq_from_nonneighbour_on_long_side(WEST))
     return 1;
  if (west_unavailable && is_recv_brq_from_nonneighbour_on_long_side(EAST))
     return 1;
  if (is_recv_brq_from_nonneighbours_both_sides(aps))
     return 1;
  return 0;
}

// pls ensure that the array size is not less than MAX_NODES_IN_RING
// the real number of isolated nodes will return by 'num'
void get_isolated_nodes(struct aps_controller* aps, int isolated_nodes[], int* num) {
    assert(aps && isolated_nodes && num >= MAX_NODES_IN_RING);
    //if ()
}
