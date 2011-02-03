#ifndef NID_RESOLVER_H
#define NID_RESOLVER_H

typedef struct _nid_entry {
	u32 old;
	u32 new;
} nid_entry;

typedef struct _resolver_config {
	char *name;
	u32 nidcount;
	nid_entry *nidtable;
} resolver_config;

extern resolver_config nid_fix[];
extern u32 nid_fix_size;
u32 resolve_nid(const char * libname, u32 nid);
void setup_nid_resolver(u32 text_addr);

#define UNKNOWNNID 0xDEADBEEF

#endif
