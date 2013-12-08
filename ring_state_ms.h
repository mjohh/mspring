#ifndef ring_state_ms_h
#define ring_state_ms_h

#ifdef __cplusplus
extern "C" {
#endif

void ms_state_run(struct aps_controller* aps);
void ms_state_exit(struct aps_controller* aps);

#ifdef __cplusplus
}
#endif

#endif //ring_state_ms_h
