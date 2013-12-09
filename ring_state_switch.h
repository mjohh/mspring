#ifndef ring_state_switch_h
#define ring_state_switch_h
#ifdef __cplusplus
extern "C" {
#endif
    
enum switch_state_id {
    SW_STARTUP = 0,
    SW_EXER,
    SW_MS,
    SW_SD,
    SW_SF,
    SW_FS,
    SW_LP,
    SW_STATE_MAX
};
struct aps_controller;
void sw_state_init(struct aps_controller *aps);
void sw_state_run(struct aps_controller *aps);
void sw_state_exit(struct aps_controller *aps);
void sw_state_fini(struct aps_controller *aps);
    
#ifdef __cplusplus
}
#endif
#endif //ring_state_switch_h
