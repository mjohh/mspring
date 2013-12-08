#include "ring_def.h"
#include "ring_state.h"
#include "ring_state_switch.h"
#include "ring_state_wtr.h"
#include "ring_state_primary.h"
#include "ring_aps_controller.h"
#include "ring_funcs.h"

#include <assert.h>
//// this file impl rule s

//// rule s#1  switching state sourced k-bytes:
//// rule s#1a switching state sourced k-bytes spec profile.
//// rule s#1b switching state should source rr in short and brq in long.
//// rule s#1c a)recv two short, reply two long.
//// rule s#1d detection has higher pri.
//// rule s#2  switching state terminating kbytes.
//// rule s#3  RR & long reply
//// rule s#4  coexist brsw
//// rule s#4b coexist SD,MS,EXER need drop brsw.
//// rule s#5
//// rule s#8
//// rule basic#3 MS-RDI has higher prio.
//// rule basic#4


//// rule s#1c
//// rule s#1d
int is_coexist_local_brq(struct aps_controller* aps)
{
    enum brq_code w_local_brq = get_local_brq(aps, WEST);
    enum brq_code e_local_brq = get_local_brq(aps, EAST);
    
	enum brq_code w_hi = get_highest_brq_on_side(aps, WEST);
	enum brq_code e_hi = get_highest_brq_on_side(aps, EAST);
    
	if(NR == w_local_brq || NR == e_local_brq)
		return 0;

	return(is_coexist_brq(w_hi, w_local_brq) && 
           is_coexist_brq(e_hi, e_local_brq) && 
		   is_coexist_brq(w_local_brq, e_local_brq));
}


int is_coexist_local_brq_and_short_brq(struct aps_controller* aps, enum side local_side)
{
    enum side other_side = OTHER_SIDE(local_side);
    enum brq_code local_brq = get_local_brq(aps, local_side);
	enum brq_code local_hi = get_highest_brq_on_side(aps, local_side);
	enum brq_code other_hi = get_highest_brq_on_side(aps, other_side);
	enum brq_code other_brq = get_kbyte_brq_for_me(aps, other_side);

	if(NR == local_brq || NR == other_brq)
		return 0;

	return is_coexist_brq(local_hi, local_brq) && 
		   SHORT_PATH == DRV_KBYTES_PATH(other_side) && other_hi == other_brq &&
		   is_coexist_brq(other_brq, local_brq);
}


int is_only_local_brq(struct aps_controller* aps, enum side local_side)
{
    enum side other_side = OTHER_SIDE(local_side);
    enum brq_code local_brq = get_local_brq(aps, local_side);
	enum brq_code local_hi = get_highest_brq_on_side(aps, local_side);
	enum brq_code other_hi = get_highest_brq_on_side(aps, other_side);
	enum path_type other_path = DRV_KBYTES_PATH(other_side);

	if(NR == local_brq)
		return 0;
    
	if(SHORT_PATH == other_path && other_hi < local_brq && !is_coexist_brq(other_hi, local_brq) && is_coexist_brq(local_hi, local_brq))
	{
	    return 1;	
	}

	if(LONG_PATH == other_path && (other_hi <= local_brq || is_coexist_brq(other_hi, local_brq) /*|| is_ack_for_me*/) && is_coexist_brq(local_hi, local_brq))
	{
	    return 1;
	}
	return 0;
}


int is_coexist_short_brq(struct aps_controller* aps)
{
	enum brq_code w_hi = get_highest_brq_on_side(aps, WEST);
	enum brq_code e_hi = get_highest_brq_on_side(aps, EAST);
	enum brq_code w_brq = get_kbyte_brq_for_me(aps, WEST);
	enum brq_code e_brq = get_kbyte_brq_for_me(aps, EAST);

	if(NR == w_brq || NR == e_brq)
		return 0;
    if(DRV_KBYTES_PATH(WEST) == SHORT_PATH && DRV_KBYTES_PATH(EAST) == SHORT_PATH)
    {   
        if(!(DRV_KBYTES_DEST(WEST) == MY_NODE_ID && DRV_KBYTES_DEST(EAST) == MY_NODE_ID &&
                 DRV_KBYTES_SRC(WEST) == WEST_NODE_ID && DRV_KBYTES_SRC(EAST)== EAST_NODE_ID))
		{
		    return 0;	
		}
        return (w_brq == w_hi &&
			    e_brq == e_hi &&
			    is_coexist_brq(w_brq, e_brq));
    }
    return 0;
}


int is_only_short_brq(struct aps_controller* aps, enum side side)
{
	enum side other_side = OTHER_SIDE(side);
	enum brq_code kbrq_for_me_other_side = get_kbyte_brq_for_me(aps, other_side);
	enum brq_code hi_brq = get_highest_brq(aps);
	enum brq_code local_brq = get_local_brq(aps, side);
    enum brq_code local_brq_other_side = get_local_brq(aps, other_side);
	enum brq_code short_brq_other_side = get_short_brq(aps, other_side);
	enum brq_code short_brq = get_short_brq(aps, side);
	enum brq_code long_brq_other_side = get_long_brq(aps, other_side);

	if(NR == short_brq)
		return 0;

	if(short_brq > local_brq &&
	   short_brq > local_brq_other_side && short_brq > short_brq_other_side &&
	   !is_coexist_brq(short_brq, local_brq_other_side) && !is_coexist_brq(short_brq, short_brq_other_side) &&
	   (short_brq > long_brq_other_side || is_coexist_brq(short_brq, long_brq_other_side)))
	{
	    return 1;	
	}

	if(LONG_PATH==DRV_KBYTES_PATH(other_side) && kbrq_for_me_other_side!=NR && 
	   kbrq_for_me_other_side==hi_brq && !is_coexist_brq(kbrq_for_me_other_side, local_brq) && 
	   kbrq_for_me_other_side>local_brq_other_side)
	{
	    return 1;
	}

	return 0;
}

//// the following update the sourced kbytes for conditions mentioned aboved.
void update_for_coexist_local_brq(struct aps_controller* aps)
{
    enum brq_code w_local_brq = get_local_brq(aps, WEST);
    enum brq_code e_local_brq = get_local_brq(aps, EAST);
	enum status w_status = (SF == w_local_brq ? RDI_STATUS : IDLE_STATUS);
	enum status e_status = (SF == e_local_brq ? RDI_STATUS : IDLE_STATUS);
 
	UPDATE_CURRENT_KBYTES(WEST, w_local_brq, WEST_NODE_ID, MY_NODE_ID, SHORT_PATH, w_status);
	UPDATE_CURRENT_KBYTES(EAST, e_local_brq, EAST_NODE_ID, MY_NODE_ID, SHORT_PATH, e_status);    
}


void update_for_coexist_local_brq_and_short_brq(struct aps_controller* aps, enum side local_side)
{
    enum brq_code local_brq = get_local_brq(aps, local_side);
    enum status local_status = (SF == local_brq ? RDI_STATUS : IDLE_STATUS);
    enum side other_side = OTHER_SIDE(local_side);
    
    UPDATE_CURRENT_KBYTES(local_side, local_brq, NEIB_NODE_ID(local_side), MY_NODE_ID, SHORT_PATH, local_status);
	UPDATE_CURRENT_KBYTES(other_side, local_brq, NEIB_NODE_ID(local_side), MY_NODE_ID, LONG_PATH, IDLE_STATUS);  
}


void update_for_only_local_brq(struct aps_controller* aps, enum side local_side, enum status status)
{
    enum brq_code local_brq = get_local_brq(aps, local_side);
    enum status local_status = (SF == local_brq ? RDI_STATUS : status);
    enum side other_side = OTHER_SIDE(local_side);
    
    UPDATE_CURRENT_KBYTES(local_side, local_brq, NEIB_NODE_ID(local_side), MY_NODE_ID, SHORT_PATH, local_status);
	UPDATE_CURRENT_KBYTES(other_side, local_brq, NEIB_NODE_ID(local_side), MY_NODE_ID, LONG_PATH, status);  
}


void update_for_coexist_short_brq(struct aps_controller* aps)
{
     UPDATE_CURRENT_KBYTES(WEST, DRV_KBYTES_BRCODE(EAST), EAST_NODE_ID, MY_NODE_ID, LONG_PATH, IDLE_STATUS);
     UPDATE_CURRENT_KBYTES(EAST, DRV_KBYTES_BRCODE(WEST), WEST_NODE_ID, MY_NODE_ID, LONG_PATH, IDLE_STATUS);    
}


void update_for_only_short_brq(struct aps_controller* aps, enum side side, enum status status, enum status ack_status)
{
     enum side other_side = OTHER_SIDE(side);
     UPDATE_CURRENT_KBYTES(side, RR, NEIB_NODE_ID(side), MY_NODE_ID, SHORT_PATH, status);
     UPDATE_CURRENT_KBYTES(other_side, DRV_KBYTES_BRCODE(side), NEIB_NODE_ID(side), MY_NODE_ID, LONG_PATH, ack_status); 
}


int is_different_span_for_local_sd_ms_exer(struct aps_controller* aps, enum side side)
{
    enum side other_side = OTHER_SIDE(side);
	enum brq_code local_brq = get_local_brq(aps, side);
    
    return (is_only_local_brq(aps, side) && 
           (local_brq == SD || local_brq == MS || local_brq == EXER) &&
           (DRV_KBYTES_BRCODE(other_side) == SD || DRV_KBYTES_BRCODE(other_side) == MS || DRV_KBYTES_BRCODE(other_side) == EXER) &&
            DRV_KBYTES_PATH(other_side) == LONG_PATH && DRV_KBYTES_DEST(other_side) != MY_NODE_ID);
}

int is_different_span_for_short_sd_ms_exer(struct aps_controller* aps, enum side side)
{
    enum side other_side = OTHER_SIDE(side);
    
    return (is_only_short_brq(aps, side) && 
           (DRV_KBYTES_BRCODE(side) == SD || DRV_KBYTES_BRCODE(side) == MS || DRV_KBYTES_BRCODE(side) == EXER) &&
           (DRV_KBYTES_BRCODE(other_side) == SD || DRV_KBYTES_BRCODE(other_side) == MS || DRV_KBYTES_BRCODE(other_side) == EXER) &&
            DRV_KBYTES_PATH(other_side) == LONG_PATH && DRV_KBYTES_DEST(other_side) != MY_NODE_ID);
}

void update_for_different_span_for_local_sd_ms_exer(struct aps_controller* aps, enum side side)
{
    enum side other_side = OTHER_SIDE(side);
    
    UPDATE_CURRENT_KBYTES(side, get_local_brq(aps, side), NEIB_NODE_ID(side), MY_NODE_ID, SHORT_PATH, IDLE_STATUS);
    UPDATE_CURRENT_KBYTES(other_side, get_local_brq(aps, side), NEIB_NODE_ID(side), MY_NODE_ID, LONG_PATH, IDLE_STATUS);    
}

void update_for_different_span_for_short_sd_ms_exer(struct aps_controller* aps, enum side side)
{
    enum side other_side = OTHER_SIDE(side);
    
	UPDATE_CURRENT_KBYTES(side, RR, NEIB_NODE_ID(side), MY_NODE_ID, SHORT_PATH, IDLE_STATUS);
    UPDATE_CURRENT_KBYTES(other_side, DRV_KBYTES_BRCODE(side), NEIB_NODE_ID(side), MY_NODE_ID, LONG_PATH, IDLE_STATUS);
}


int is_isolated_node(struct aps_controller* aps)
{
	if(is_coexist_local_brq(aps))
	{
	    return 1;
	}
	if(is_coexist_local_brq_and_short_brq(aps, WEST))
	{
        return 1;
	}
	if(is_coexist_local_brq_and_short_brq(aps, EAST))
	{
		return 1;
	}
    if(is_coexist_short_brq(aps))
    {
        return 1;
    }
	return 0;
}

void update_kbytes_for_isolated_node(struct aps_controller* aps)
{
	if(is_coexist_local_brq(aps))
	{
	    update_for_coexist_local_brq(aps);
	}
	else if(is_coexist_local_brq_and_short_brq(aps, WEST))
	{
        update_for_coexist_local_brq_and_short_brq(aps, WEST);
	}
	else if(is_coexist_local_brq_and_short_brq(aps, EAST))
	{
		update_for_coexist_local_brq_and_short_brq(aps, EAST);
	}
    else if(is_coexist_short_brq(aps))
    {
        update_for_coexist_short_brq(aps);
    }
	else
	{
	    assert(0);
	}
}


