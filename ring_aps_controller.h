#ifndef mspring_ring_aps_controller_h
#define mspring_ring_aps_controller_h
#ifdef __cplusplus
extern "C" {
#endif
    
struct aps_controller {
    // input
    struct k1k2 drv_kbytes[NUM_SIDES];
    enum dq dq[NUM_SIDES];
    enum ext_cmd ext_cmd;
    enum side ext_side;
    int is_ne_ready; // before ready it src default kbytes.
    int is_wtr_timeout; // when wtr time out or extcmd release it 's TRUE.
    // output
    struct k1k2 cur_kbytes[NUM_SIDES];
    int is_wtr_start; // when need start wtr timer, set it TRUE.
    enum node_state node_state;
    // status
    enum side tail_side; // for wtr proc
    enum side short_side;
    enum primary_state_id cur_prim_state;
    enum switch_state_id cur_sw_state;
    enum end_state_id cur_end_state;
    // cfg
    unsigned short ring_map[MAX_NODES_IN_RING];
    int nodes_num;
    int node_id;
    int ring_id;
    int neib_id[NUM_SIDES];
    int slot[NUM_SIDES];
    int port[NUM_SIDES];
    int wtr_time; // second
};

#define CURRENT_KBYTES(side)         aps->cur_kbytes[side]
#define CURRENT_KBYTES_K1(side)      aps->cur_kbytes[side].k1
#define CURRENT_KBYTES_K2(side)      aps->cur_kbytes[side].k2
#define UPDATE_DEST(side,dest)       aps->cur_kbytes[side].k1.dest_node = dest
#define UPDATE_BRQ(side,brq)         aps->cur_kbytes[side].k1.brcode = brq
#define UPDATE_SRC(side,src)         aps->cur_kbytes[side].k2.src_node = src
#define UPDATE_PATH(side,pth)        aps->cur_kbytes[side].k2.path = pth
#define UPDATE_STATUS(side,stat)     aps->cur_kbytes[side].k2.status = stat
#define UPDATE_CURRENT_KBYTES(side, brq, dest, src, pth, stat)\
do{\
    aps->cur_kbytes[side].k1.brcode = brq;\
    aps->cur_kbytes[side].k1.dest_node = dest;\
    aps->cur_kbytes[side].k2.src_node = src;\
    aps->cur_kbytes[side].k2.path = pth;\
    aps->cur_kbytes[side].k2.status = stat;\
}while(0)

#define DRV_KBYTES(side)             aps->drv_kbytes[side]
#define DRV_KBYTES_K1(side)          aps->drv_kbytes[side].k1
#define DRV_KBYTES_K2(side)          aps->drv_kbytes[side].k2
#define DRV_KBYTES_BRCODE(side)      aps->drv_kbytes[side].k1.brcode
#define DRV_KBYTES_DEST(side)        aps->drv_kbytes[side].k1.dest_node
#define DRV_KBYTES_SRC(side)         aps->drv_kbytes[side].k2.src_node
#define DRV_KBYTES_PATH(side)        aps->drv_kbytes[side].k2.path
#define DRV_KBYTES_STATUS(side)      aps->drv_kbytes[side].k2.status

#define MY_NODE_ID                   aps->node_id
#define WEST_NODE_ID                 aps->neib_id[WEST]
#define EAST_NODE_ID                 aps->neib_id[EAST]
#define NEIB_NODE_ID(side)           aps->neib_id[side]

#define DRV_DQ(side)                 aps->dq[side]
#define EXT_CMD_CMD                  aps->ext_cmd
#define EXT_CMD_SIDE                 aps->ext_side

#define MY_SLOT(side)               aps->slot[side]
#define MY_PORT(side)               aps->port[side]

#define IS_NE_READY                 aps->is_ne_ready

#define UPDATE_KBYTES(kbytes, side, brq, dest, src, pth, stat)\
do{\
    kbytes[side].k1.brcode = brq;\
    kbytes[side].k1.dest_node = dest;\
    kbytes[side].k2.src_node = src;\
    kbytes[side].k2.path = pth;\
    kbytes[side].k2.status = stat;\
}while(0)
int aps_controller_init(struct aps_controller* aps);
void aps_controller_fini(struct aps_controller* aps);
void aps_controller_run(struct aps_controller* aps);

void aps_input_kbyte(struct aps_controller * aps, enum side side, struct k1k2 * k1k2);
void aps_input_dq(struct aps_controller * aps, enum side side, enum dq dq);
void aps_input_ext_cmd(struct aps_controller * aps, enum side side, enum ext_cmd ext_cmd);
void aps_input_ne_ready_flag(struct aps_controller * aps, int is_ne_ready);
void aps_input_wtr_timeout_flag(struct aps_controller * aps, int is_wtr_timeout);
    
void aps_output(struct aps_controller* aps,
               int (*output_set_kbyte)(int ringid, int slot, int port, struct k1k2 * k1k2),
               int (*output_update_hw)(int ringid, int slot[NUM_SIDES], int port[NUM_SIDES], enum node_state state),
               int (*output_start_wtr)(int ringid, int enable, int sec));
    
int aps_cfg(struct aps_controller * aps,
            unsigned short ring_map[MAX_NODES_IN_RING],
            int nodes_num,
            int node_id,
            int ring_id,
            int slot[NUM_SIDES],
            int port[NUM_SIDES],
            int wtr_time);

#ifdef __cplusplus
    }
#endif
#endif //mspring_ring_aps_controller_h
