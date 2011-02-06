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
resolver_config* get_nid_resolver(const char *libname);
u32 resolve_nid(resolver_config *resolver, u32 nid);
void setup_nid_resolver(void);
void resolve_syscon_driver(SceModule *mod);

#define UNKNOWNNID 0xDEADBEEF

#endif
