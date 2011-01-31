#ifndef LIBS_H
#define LIBS_H

typedef struct
{
	const char *name;
	unsigned short version;
	unsigned short attribute;
	unsigned char entLen;
	unsigned char varCount;
	unsigned short funcCount;
	unsigned int *fnids;
	unsigned int *funcs;
	unsigned int *vnids;
	unsigned int *vars;
}PspModuleImport;

PspModuleImport *find_import_lib(SceModule *pMod, char *library);

unsigned int find_import_bynid(SceModule *pMod, char *library, unsigned int nid);

void api_hook_addr(int addr, void *func);
void api_hook_import(int addr, void *func);

int hook_import_bynid(SceModule *pMod, char *library, unsigned int nid, void *func, int syscall);

#endif
