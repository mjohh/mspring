
// only sf and fs need squelch
int is_interface_unavailabe(struct aps_controller* aps, enume side side) {
    assert(aps);
    assert(WEST == side || EAST == side);
    if (is_cut(aps, side) || (EXT_CMD_CMD == EX_FS && EXT_CMD_SIDE == side))// tail
        return 1;
    if (get_short_brq(aps, side) == SF || get_short_brq(aps, side) == FS)// head
        return 1;
    return 0;
}
