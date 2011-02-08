#pragma once

extern u8 *rebootex;
extern u32 rebootex_size;

extern char *g_insert_module_after;
extern void *g_insert_module_binary;
extern int g_insert_module_size;
extern int g_insert_module_flags;

int on_module_start(SceModule2 *mod);
void patch_sceLoadCore(void);
void patch_sceLoadExec(void);
int load_rebootex_from_ms0(void);
void patch_sceInterruptManager(void);
void patch_partitions(void);
