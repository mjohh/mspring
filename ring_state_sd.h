#ifndef ring_state_sd_h
#define ring_state_sd_h
#ifdef __cplusplus
extern "C" {
#endif

void sd_state_run(struct aps_controller* aps);
void sd_state_exit(struct aps_controller* aps);

#ifdef __cplusplus
}
#endif
#endif //ring_state_sd_h
