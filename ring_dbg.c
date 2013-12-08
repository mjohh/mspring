#include "ring_state_primary.h"
#include "ring_state_switch.h"
#include "ring_state_sf.h"
#include "ring_def.h"
#include "ring_dbg.h"
#include "ring_funcs.h"

#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdarg.h>

int g_print_enable[MAX_NODE_ID] = { 0 };

void ring_print_disable(int nodeid)
{
    assert(nodeid < MAX_NODE_ID);
    g_print_enable[nodeid] = 0;
}

void ring_print_enable(int nodeid)
{
    assert(nodeid < MAX_NODE_ID);
    g_print_enable[nodeid] = 1;
}

int is_print_enable(int nodeid)
{
    return g_print_enable[nodeid];
}

void ring_print(int nodeid, const char *fmt, ...)
{
    char buf[MAXLINE + 1];
    char *p = NULL;
    va_list ap;
    
    if (!is_print_enable(nodeid)) {
        return;
    }
    
    sprintf(buf, "[%d]", nodeid);
    p = buf;
    p += strlen(buf);
    va_start(ap, fmt);
    
    vsnprintf(p + strlen(p), MAXLINE, fmt, ap);
    printf(buf);
    
    va_end(ap);
}

void ring_print_kbytes(int nodeid, struct k1k2 *kbytes)
{
    ring_print(nodeid, "[%s,%d|%d,%d,%s]n", get_name_of_brq(kbytes->k1.brcode), kbytes->k1.dest_node, kbytes->k2.src_node, kbytes->k2.path, get_name_of_status(kbytes->k2.status));
}
