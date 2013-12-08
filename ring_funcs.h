#ifndef ring_funcs_h
#define ring_funcs_h

#ifdef __cplusplus
extern "C" {
#endif

enum brq_code get_highest_brq_on_side(struct aps_controller* aps, enum side side);

enum brq_code get_highest_brq(struct aps_controller* aps);

int is_coexist_brq(enum brq_code wbrq, enum brq_code ebrq);

enum brq_code get_local_brq(struct aps_controller* aps, enum side side);

#define OTHER_SIDE(side) ((side)==WEST ? EAST : WEST)

enum side get_side_by_slot_port(struct aps_controller* aps, int slot, int port);

// wait for impl
int is_leagal_kbytes(struct aps_controller* aps, struct k1k2* k1k2);

//void tx_current_kbytes(struct aps_controller* aps);

enum brq_code get_kbyte_brq(struct aps_controller* aps, enum side side);

void update_node_state(struct aps_controller* aps, enum node_state state);

enum brq_code get_highest_brq_for_me(struct aps_controller* aps);

enum brq_code get_kbyte_brq_for_me(struct aps_controller* aps, enum side side);

int is_kbyte_for_me(struct aps_controller* aps, enum side side);

enum brq_code get_kbyte_brq_for_other(struct aps_controller* aps, enum side side);

enum brq_code get_highest_brq_for_other(struct aps_controller* aps);

int is_cut(struct aps_controller* aps, enum side side);

int are_kbytes_readable(struct aps_controller* aps, enum side side);

enum brq_code get_dq_brq(struct aps_controller* aps, enum side side);

enum brq_code get_ext_brq(struct aps_controller* aps);

enum brq_code get_brq_for_me(struct aps_controller* aps, enum side side);

enum brq_code get_brq_for_other(struct aps_controller* aps, enum side side);

enum brq_code get_short_brq(struct aps_controller* aps, enum side side);

enum brq_code get_long_brq(struct aps_controller* aps, enum side side);

char* get_name_of_brq(enum brq_code brq);

char* get_name_of_status(enum status stat);

char * dq_name(enum dq dq);

int is_valid_long_reply_for_brq(struct aps_controller* aps, enum brq_code brq, enum side reply_side);

int is_tail_end(struct aps_controller* aps);

int is_rcv_nr_idle_both_sides(struct aps_controller* aps);

int is_default_kbytes(struct k1k2* kbyte);

#ifdef __cplusplus
}
#endif

#endif //ring_funcs_h