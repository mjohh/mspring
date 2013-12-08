#ifndef ring_dbg_h
#define ring_dbg_h


#ifdef __cplusplus
extern "C" {
#endif

#define MAXLINE 256
void ring_print(int nodeid, const char *fmt, ...);
void ring_print_kbytes(int nodeid, struct k1k2* kbytes);
void ring_print_disable(int nodeid);
void ring_print_enable(int nodeid);

#ifdef __cplusplus
}
#endif

#endif //ring_dbg_h
