#ifndef ring_state_lp_h
#define ring_state_lp_h
#ifdef __cplusplus
extern "C" {
#endif

void lp_state_run(struct aps_controller* aps);
void lp_state_exit(struct aps_controller* aps);

#ifdef __cplusplus
}
#endif
#endif //ring_state_lp_h