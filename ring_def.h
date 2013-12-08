#ifndef ring_def_h
#define ring_def_h

#ifdef __cplusplus
extern "C" {
#endif

enum side
{
    WEST,
    EAST,
    UNDEFINED_SIDE,
    NUM_SIDES = UNDEFINED_SIDE
};


enum dq
{
    DQ_CLEAR = 0,
    DQ_SD = 1,
    DQ_SF = 2,
    DQ_K1K2_INCONSISTENCY = 3,
    NUM_DQ_CODES = 4
};


struct k1
{
    unsigned int brcode:4;
	unsigned int dest_node:4;
};


struct k2
{
	unsigned int src_node:4;
	unsigned int path:1;
	unsigned int status:3;
};


struct k1k2
{
	struct k1 k1;
	struct k2 k2;
};


#define INVALID_KBYTE     0xffff
#define DEFAULT_K1BYTE    0x00
#define DEFAULT_K2BYTE    0x00
#define MAX_NODES_IN_RING 16
#define ILLEGAL_NODE_ID    0xffff

#define MIN_NODE_ID 0
#define MAX_NODE_ID 15

#define NUM_BRQ_SOURCES 5
/*
bridge request code
*/
enum brq_code
{
    NR = 0,
    RR = 1,
    RR_S = 2,
    EXER = 3,
    EXER_S = 4,
    WTR = 5,
    MS = 6,
    MS_S = 7,
    SD = 8,
    SD_S = 9,
    SD_P = 10,
    SF = 11,
    SF_S = 12,
    FS = 13,
    FS_S = 14,
    LOP = 15,
    NUM_BR_CODES = 16
};

/*
microsoft uses these defines
*/
#ifndef MSC
enum path_type
{
    SHORT_PATH = 0,
    LONG_PATH = 1
};
#endif

enum status
{
    IDLE_STATUS = 0,
    BR_STATUS = 1,
    BRSW_STATUS = 2,
    ET = 3,
    RESV1 = 4,
    IDLE_ACK_STATUS = RESV1,
    RESV2 = 5,
    NOT_IDLE_ACK_STATUS = RESV2,
    RDI_STATUS = 6,
    MS_AIS = 7,
    NUM_STATUSES = 8
};


enum node_state
{
    IDLE,
    PASS,
    k_PASS,
    BR_E,
    BR_W,
    BRSW_E,
    BRSW_W,
    DISABLE_SESSION,
    ILLEGAL_STATE
};

enum ext_cmd
{
    EX_NONE,
    EX_LOW,
    EX_LOP,
    EX_FS,
    EX_MAN,
    EX_EXER,
    EX_CLEAR,
    NUM_EXT_CMDS
};
    
#define RMAX(a,b) ((a)>(b)?(a):(b))

#ifdef __cplusplus
}
#endif

#endif //ring_def_h

