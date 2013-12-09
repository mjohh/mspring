#ifndef ring_state_sw_upgrd_h
#define ring_state_sw_upgrd_h
#ifdef __cplusplus
extern "C" {
#endif

// upgrade,degrade or occupy if needed.
void drop_switch_if_occupy(struct aps_controller* aps);
void get_reason_brq_and_short_side(struct aps_controller* aps, enum brq_code* brq, enum side* side);
void exit_cur_sw_state(struct aps_controller* aps);

#ifdef __cplusplus
}
#endif
#endif //ring_state_sw_upgrd_h
