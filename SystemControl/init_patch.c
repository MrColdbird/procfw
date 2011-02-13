#include <pspsdk.h>
#include <pspsysmem_kernel.h>
#include <pspkernel.h>
#include <psputilsforkernel.h>
#include <pspsysevent.h>
#include <pspiofilemgr.h>
#include <stdio.h>
#include <string.h>
#include "main.h"
#include "utils.h"
#include "systemctrl.h"
#include "printk.h"
#include "libs.h"
#include "strsafe.h"

struct InitHookFunction {
	u32 nid;
	void *fp;
	u32 *fp_orig;
};

static SceUID (*_sceKernelLoadModuleUMDEmu)(int apitype, const char * path, int flags, SceKernelLMOption * option);
static SceUID (*_sceKernelLoadModuleDisc)(const char * path, int flags, SceKernelLMOption * option);
static SceUID (*_sceKernelLoadModuleEf0)(int apitype, const char * path, int flags, SceKernelLMOption * option);
static SceUID (*_sceKernelLoadModuleM2)(int apitype, const char * path, int flags, SceKernelLMOption * option);

static int get_game_tag(const char *path, u32 *tag)
{
	SceUID fd;
	char buf[0x150];
	int ret;

	if(path == NULL) {
		return -1;
	}

	fd = sceIoOpen(path, PSP_O_RDONLY, 0777);

	if(fd < 0) {
		return fd;
	}

	ret = sceIoRead(fd, buf, sizeof(buf));
	
	if(ret != sizeof(buf)) {
		ret = -2;
		goto exit;
	}

	if(*(u32*)buf != 0x5053507E) /* ~PSP */ {
		ret = -3;
		goto exit;
	}

	*tag = *(u32*)(buf+0xD0);
	ret = 0;

exit:
	sceIoClose(fd);

	return ret;
}

static int load_opnssmp(const char *path, u32 tag)
{
	SceUID modid;
	int opnssmp_type, ret;
	char opnssmp_path[128], *p;

	opnssmp_type = (tag >> 8) & 0xFF;
	STRCPY_S(opnssmp_path, path);
	p = strrchr(opnssmp_path, '/'); 
	
	if (p != NULL) {
		p[1] = '\0';
	} else {
		opnssmp_path[0] = '\0';
	}

	STRCAT_S(opnssmp_path, "OPNSSMP.BIN");
	modid = sceKernelLoadModule(opnssmp_path, 0, NULL);

	if(modid < 0) {
		printk("%s: load %s -> 0x%08X\n", __func__, opnssmp_path, modid);

		return modid;
	}

	ret = sceKernelStartModule(modid, 4, &opnssmp_type, 0, NULL);

	if(ret < 0) {
		printk("%s: start module -> 0x%08X\n", __func__, modid);
		sceKernelUnloadModule(modid);

		return ret;
	}

	return modid;
}

static SceUID myKernelLoadModuleUMDEmu(int apitype, const char * path, int flags, SceKernelLMOption * option) 
{
	SceUID ret;
	u32 tag;
	int retv;

	retv = get_game_tag(path, &tag);

	if (retv == 0) {
		printk("%s: tag 0x%08X\n", __func__, tag);
		load_opnssmp(path, tag);
	} else {
		printk("%s: get_game_tag -> 0x%08X\n", __func__, retv);
	}

	ret = _sceKernelLoadModuleUMDEmu(apitype, path, flags, option);
	printk("%s: %d %s -> 0x%08X\n", __func__, apitype, path, ret);

	return ret;
}

static SceUID myKernelLoadModuleDisc(const char * path, int flags, SceKernelLMOption * option) 
{
	SceUID ret;
	u32 tag;
	int retv;

	retv = get_game_tag(path, &tag);

	if (retv == 0) {
		printk("%s: tag 0x%08X\n", __func__, tag);
		load_opnssmp(path, tag);
	} else {
		printk("%s: get_game_tag -> 0x%08X\n", __func__, retv);
	}

	ret = _sceKernelLoadModuleDisc(path, flags, option);
	printk("%s: -> 0x%08X\n", __func__, ret);

	return ret;
}

static SceUID myKernelLoadModuleEf0(int apitype, const char * path, int flags, SceKernelLMOption * option) 
{
	SceUID ret;
	u32 tag;
	int retv;

	retv = get_game_tag(path, &tag);

	if (retv == 0) {
		printk("%s: tag 0x%08X\n", __func__, tag);
		load_opnssmp(path, tag);
	} else {
		printk("%s: get_game_tag -> 0x%08X\n", __func__, retv);
	}

	ret = _sceKernelLoadModuleEf0(apitype, path, flags, option);
	printk("%s: %d %s -> 0x%08X\n", __func__, apitype, path, ret);

	return ret;
}

static SceUID myKernelLoadModuleM2(int apitype, const char * path, int flags, SceKernelLMOption * option) 
{
	SceUID ret;
	u32 tag;
	int retv;

	retv = get_game_tag(path, &tag);

	if (retv == 0) {
		printk("%s: tag 0x%08X\n", __func__, tag);
		load_opnssmp(path, tag);
	} else {
		printk("%s: get_game_tag -> 0x%08X\n", __func__, retv);
	}

	ret = _sceKernelLoadModuleM2(apitype, path, flags, option);
	printk("%s: %d %s -> 0x%08X\n", __func__, apitype, path, ret);

	return ret;
}

static u32 get_modulemgr_function(u32 nid)
{
	return sctrlHENFindFunction("sceModuleManager", "ModuleMgrForKernel", nid);
}

static struct InitHookFunction g_init_functions[] = {
	{ 0x32292450, myKernelLoadModuleUMDEmu, (u32*)&_sceKernelLoadModuleUMDEmu, }, // 0x114
	{ 0xA8D1F46E, myKernelLoadModuleM2, (u32*)&_sceKernelLoadModuleM2, }, // 0x124
	{ 0x4986499C, myKernelLoadModuleDisc, (u32*)&_sceKernelLoadModuleDisc, }, // 0x120
	{ 0xCC77F137, myKernelLoadModuleEf0, (u32*)&_sceKernelLoadModuleEf0, }, // 0x125
};

void patch_sceInit(void)
{
	SceModule *mod;
	int i;

	mod = sceKernelFindModuleByName("sceInit");

	if(mod == NULL) {
		printk("%s: sceInit not found\n", __func__);

		return;
	}

	for(i=0; i<NELEMS(g_init_functions); ++i) {
		*g_init_functions[i].fp_orig = get_modulemgr_function(g_init_functions[i].nid);
		hook_import_bynid(mod, "ModuleMgrForKernel", g_init_functions[i].nid, g_init_functions[i].fp, 0);
	}
}
