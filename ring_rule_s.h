#ifndef ring_rule_s_h
#define ring_rule_s_h
#ifdef __cplusplus
extern "C" {
#endif

//// rule s#1  switching state sourced k-bytes:
//// rule s#1a switching state sourced k-bytes spec profile.
//// rule s#1b switching state should source rr in short and brq in long.
//// rule s#1c
//// rule s#1d
//// rule s#2
//// rule s#3
//// rule s#4
//// rule s#4b
//// rule s#5
//// rule s#8
//// rule basic#3
//// rule basic#4
//// rule s#1c
//// rule s#1d
int is_coexist_local_brq(struct aps_controller *aps);
int is_coexist_local_brq_and_short_brq(struct aps_controller *aps, enum side local_side);
int is_only_local_brq(struct aps_controller *aps, enum side local_side);
int is_coexist_short_brq(struct aps_controller *aps);
int is_only_short_brq(struct aps_controller *aps, enum side side);
//// the following update the sourced kbytes
//// for conditions mentioned aboved.
void update_for_coexist_local_brq(struct aps_controller *aps);
void update_for_coexist_local_brq_and_short_brq(struct aps_controller *aps, enum side local_side);
void update_for_only_local_brq(struct aps_controller *aps, enum side local_side, enum status status);
void update_for_coexist_short_brq(struct aps_controller *aps);
void update_for_only_short_brq(struct aps_controller *aps, enum side side, enum status status, enum status ack_status);
int is_different_span_for_local_sd_ms_exer(struct aps_controller *aps, enum side side);
int is_different_span_for_short_sd_ms_exer(struct aps_controller *aps, enum side side);
void update_for_different_span_for_local_sd_ms_exer(struct aps_controller *aps, enum side side);
void update_for_different_span_for_short_sd_ms_exer(struct aps_controller *aps, enum side side);
int is_isolated_node(struct aps_controller *aps);
void update_kbytes_for_isolated_node(struct aps_controller *aps);

#ifdef __cplusplus
}
#endif
#endif //ring_rule_s_h
