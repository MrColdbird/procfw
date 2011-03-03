#ifndef STARGATE_H
#define STARGATE_H

extern SEConfig conf;

int myPauth_98B83B5D(u8 *p, u32 size, u32 *newsize, u8 *xor_key);
int myPauth_init(void);

void patch_sceMesgLed(void);

int nodrm_init(void);
void patch_drm_imports(SceModule *mod);
void patch_utility(SceModule *mod);
int nodrm_get_normal_functions(void);
int nodrm_get_npdrm_functions(void);
int load_module_get_function(void);
void patch_load_module(SceModule *mod);

extern int (*mesgled_decrypt)(u32 *tag, u8 *key, u32 code, u8 *prx, u32 size, u32 *newsize, u32 use_polling, u8 *blacklist, u32 blacklistsize, u32 type, u8 *xor_key1, u8 *xor_key2);

#endif
