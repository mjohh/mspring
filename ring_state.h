#ifndef ring_state_h
#define ring_state_h

#ifdef __cplusplus
extern "C" {
#endif


enum end_state_id
{
    END_START_UP = 0,
    END_IDLE_AND_WAITING,
    END_BR_AND_WAITING,
    END_BRSW,
    END_STATE_MAX
};
struct aps_controller;
void end_state_init(struct aps_controller* aps);
void end_state_fini(struct aps_controller* aps);
void end_state_set(struct aps_controller* aps, enum end_state_id next_state);
void end_state_run(struct aps_controller * aps);
void end_state_exit(struct aps_controller* aps);
    
void start_up_change(struct aps_controller* aps);
void idle_and_waiting_change(struct aps_controller* aps);
void br_and_waiting_change(struct aps_controller* aps);

void to_start_up(struct aps_controller * aps);
void head_to_brsw0(struct aps_controller * aps);
void head_to_brsw(struct aps_controller * aps);
void tail_to_brsw(struct aps_controller * aps);
void head_to_idle_and_waiting(struct aps_controller * aps);
void tail_to_idle_and_waiting(struct aps_controller * aps);
void head_to_br_and_waiting(struct aps_controller * aps);
void tail_to_br_and_waiting(struct aps_controller * aps);
void brsw_change(struct aps_controller* aps);

int is_coexist_sd_ms_exer(struct aps_controller* aps);
void to_start_up_for_coexist(struct aps_controller* aps);

char* end_state_name(int state);

#ifdef __cplusplus
}
#endif

#endif //ring_state_h
