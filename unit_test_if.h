#ifndef __UNIT_TEST_H__
#define __UNIT_TEST_H__

#define UPDATE_RX_KBYTES(side, brq, dest, src, pth, stat)\
do{\
	aps->drv_kbytes[side].k1.brcode = brq;\
	aps->drv_kbytes[side].k1.dest_node = dest;\
	aps->drv_kbytes[side].k2.src_node = src;\
	aps->drv_kbytes[side].k2.path = pth;\
	aps->drv_kbytes[side].k2.status = stat;\
}while(0)

#define UPDATE_EXT_CMD(side, cmd)\
do{\
	aps->ext_cmd = cmd;\
    aps->ext_side = side;\
}while(0);

#define UPDATE_DQ(side, DQ)\
	aps->dq[side] = DQ;


#define CLEAR_INPUT \
do{\
    UPDATE_RX_KBYTES(WEST, NR, 0, 0, 0, 0);\
	UPDATE_RX_KBYTES(EAST, NR, 0, 0, 0, 0);\
	UPDATE_EXT_CMD(WEST, EX_NONE);\
    UPDATE_EXT_CMD(EAST, EX_NONE);\
    UPDATE_DQ(WEST,DQ_CLEAR);\
    UPDATE_DQ(EAST,DQ_CLEAR);\
}while(0);

void assert_tx_kbytes(const struct aps_controller * aps, int side, int brq, int dest, int src, int pth, int stat);
void unit_test_startup(struct aps_controller* aps);
void unit_test_teardown(struct aps_controller* aps);

#endif //__UNIT_TEST_H__