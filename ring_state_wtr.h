#ifndef ring_state_wtr_h
#define ring_state_wtr_h

#ifdef __cplusplus
extern "C" {
#endif
struct aps_controller;
void wtr_state_init(struct aps_controller* aps);
void wtr_state_run(struct aps_controller* aps);
void wtr_state_exit(struct aps_controller* aps);
void wtr_state_fini(struct aps_controller* aps);

#ifdef __cplusplus
}
#endif

#endif //ring_state_wtr_h