#ifndef ring_state_primary_h
#define ring_state_primary_h

#ifdef __cplusplus
extern "C" {
#endif

enum primary_state_id
{
    PRIM_START_UP = 0,
	PRIM_IDLE,
	PRIM_SWITCH,
	PRIM_WTR,
	PRIM_PASS,
	PRIM_K_PASS,
	PRIM_STATE_MAX
};
struct aps_controller;
void prim_state_init(struct aps_controller* aps);
void prim_state_run(struct aps_controller* aps);
void prim_state_exit(struct aps_controller* aps);
void prim_state_fini(struct aps_controller* aps);

#ifdef __cplusplus
}
#endif

#endif //ring_state_primary_h
