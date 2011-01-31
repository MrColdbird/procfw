#include "xmbiso.h"
#include "systemctrl.h"
#include "systemctrl_se.h"
#include "systemctrl_private.h"
#include "isoreader.h"
#include "printk.h"
#include "utils.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <pspsdk.h>
#include "virtual_pbp.h"

#define MAGIC_DFD_FOR_DELETE 0x9000
#define MAGIC_DFD_FOR_DELETE_2 0x9001

//virtual eboot detect macro
#define ISOEBOOT(file) (strlen(file) == 39 && strncmp(file + 14, "ISOGAME", 7) == 0 && strcmp(file + strlen(file) - 9, "EBOOT.PBP") == 0)
#define ISODIR(file) (strlen(file) == 29 && strncmp(file+4, "/PSP/GAME/ISOGAME", sizeof("/PSP/GAME/ISOGAME")-1) == 0)

static char g_iso_dir[128];
static char g_temp_delete_dir[128];
static int g_delete_eboot_injected = 0;

//open directory
SceUID gamedopen(const char * dirname)
{
	SceUID result;
	u32 k1;

	if(ISODIR(dirname)) {
		result = MAGIC_DFD_FOR_DELETE;
		g_delete_eboot_injected = 0;
		strncpy(g_iso_dir, dirname, sizeof(g_iso_dir));
		g_iso_dir[sizeof(g_iso_dir)-1] = '\0';
		printk("%s:<virtual> %s -> 0x%08X\n", __func__, dirname, result);
		
		return result;
	}

	if(0 == strcmp(g_temp_delete_dir, dirname)) {
		result = MAGIC_DFD_FOR_DELETE_2;
		printk("%s:<virtual> %s -> 0x%08X\n", __func__, dirname, result);
		
		return result;
	}
   
	k1 = pspSdkSetK1(0);
	result = vpbp_dopen(dirname);
	pspSdkSetK1(k1);

	printk("%s: %s -> 0x%08X\n", __func__, dirname, result);

	return result;
}

//read directory
int gamedread(SceUID fd, SceIoDirent * dir)
{
	int result;
	u32 k1;
   
	if(fd == MAGIC_DFD_FOR_DELETE || fd == MAGIC_DFD_FOR_DELETE_2) {
		if (0 == g_delete_eboot_injected) {
			u32 k1;
		   
			memset(dir, 0, sizeof(*dir));
			k1 = pspSdkSetK1(0);
			result = vpbp_getstat(g_iso_dir, &dir->d_stat);
			pspSdkSetK1(k1);

			if(fd == MAGIC_DFD_FOR_DELETE) {
				strcpy(dir->d_name, "EBOOT.PBP");
			} else {
				strcpy(dir->d_name, "_EBOOT.PBP");
			}

			g_delete_eboot_injected = 1;
			result = 1;
		} else {
			result = 0;
		}
		
		printk("%s:<virtual> 0x%08X -> 0x%08X\n", __func__, fd, result);
		
		return result;
	}

	k1 = pspSdkSetK1(0);
	result = vpbp_dread(fd, dir);
	pspSdkSetK1(k1);

	return result;
}

//directory descriptor closer
int gamedclose(SceUID fd)
{
	int result;
	u32 k1;
   
	if(fd == MAGIC_DFD_FOR_DELETE || fd == MAGIC_DFD_FOR_DELETE_2) {
		result = 0;

		if(fd == MAGIC_DFD_FOR_DELETE) {
			g_iso_dir[0] = '\0';
		}

		if(fd == MAGIC_DFD_FOR_DELETE_2) {
			g_temp_delete_dir[0] = '\0';
		}

		g_delete_eboot_injected = 0;
		printk("%s:<virtual> 0x%08X -> 0x%08X\n", __func__, fd, result);
		
		return result;
	}
	
	k1 = pspSdkSetK1(0);
	result = vpbp_dclose(fd);
	pspSdkSetK1(k1);

	return result;
}

//open file
SceUID gameopen(const char * file, int flags, SceMode mode)
{
	//forward to firmware
	SceUID result;
   
	if (ISOEBOOT(file)) {
		printk("%s: %s", __func__, file);
		u32 k1 = pspSdkSetK1(0);
		result = vpbp_open(file, flags, mode);
		pspSdkSetK1(k1);
		printk(" -> 0x%08X\n", result);
	} else {
		result = sceIoOpen(file, flags, mode);
	}

	return result;
}

//read file
int gameread(SceUID fd, void * data, SceSize size)
{
	//forward to firmware
	int result;
   
	if (vpbp_is_fd(fd)) {
		printk("%s: 0x%04X %d", __func__, fd, size);
		u32 k1 = pspSdkSetK1(0);
		result = vpbp_read(fd, data, size);
		pspSdkSetK1(k1);
		printk(" -> 0x%08X\n", result);
	} else {
		result = sceIoRead(fd, data, size);
	}

	return result;
}

//close file
int gameclose(SceUID fd)
{
	int result;

	if (vpbp_is_fd(fd)) {
		printk("%s: %04X", __func__, fd);
		u32 k1 = pspSdkSetK1(0);
		result = vpbp_close(fd);
		pspSdkSetK1(k1);
		printk(" -> 0x%08X\n", result);
	} else {
		result = sceIoClose(fd);
	}
	
	return result;
}

SceOff gamelseek(SceUID fd, SceOff offset, int whence)
{
	SceOff result = 0;

	if (vpbp_is_fd(fd)) {
		printk("%s: 0x%04X 0x%08X %d", __func__, fd, (u32)offset, whence);
		u32 k1 = pspSdkSetK1(0);
		result = vpbp_lseek(fd, offset, whence);
		pspSdkSetK1(k1);
		printk(" -> 0x%08X\n", (u32)result);
	} else {
		result = sceIoLseek(fd, offset, whence);
	}

	return result;
}

//get file status
int gamegetstat(const char * file, SceIoStat * stat)
{
	int result;
   
	//virtual iso eboot detected
	if (ISOEBOOT(file)) {
		printk("%s: %s", __func__, file);
		u32 k1 = pspSdkSetK1(0);
		result = vpbp_getstat(file, stat);
		pspSdkSetK1(k1);
		printk(" -> 0x%08X\n", result);
	} else {
		result = sceIoGetstat(file, stat);
	}

	return result;
}

//remove file
int gameremove(const char * file)
{
	int result;
   
	// file under /PSP/GAME/__DEL__XXXXXXXX
	if(0 == strncmp(file, g_temp_delete_dir, strlen(g_temp_delete_dir))) {
		result = 0;
		printk("%s:<virtual> %s -> 0x%08X\n", __func__, file, result);
		
		return result;
	}
	
	result = sceIoRemove(file);
	printk("%s: %s -> 0x%08X\n", __func__, file, result);

	return result;
}

//remove folder
int gamermdir(const char * path)
{
	int result;
   
	if (ISODIR(path)) {
		u32 k1 = pspSdkSetK1(0);
		result = vpbp_remove(path);
		pspSdkSetK1(k1);
		printk("%s:<virtual> %s -> 0x%08X\n", __func__, path, result);

		return 0;
	}

	result = sceIoRmdir(path);
	printk("%s: %s 0x%08X\n", __func__, path, result);

	return result;
}

//load and execute file
int gameloadexec(char * file, struct SceKernelLoadExecVSHParam * param)
{
	//result
	int result = 0;

	printk("%s: %s %s\n", __func__, file, param->key);
	
	//virtual iso eboot detected
	if (ISOEBOOT(file)) {
		u32 k1 = pspSdkSetK1(0);
		result = vpbp_loadexec(file, param);
		pspSdkSetK1(k1);

		return result;
	}

	//forward to ms0 handler
	if(strncmp(file, "ms", 2) == 0) result = sctrlKernelLoadExecVSHMs2(file, param);

	//forward to ef0 handler
	else result = sctrlKernelLoadExecVSHEf2(file, param);

	return result;
}

int gamerename(const char *oldname, const char *newfile)
{
	int result;

	if(ISODIR(oldname)) {
		result = 0;
		strncpy(g_temp_delete_dir, newfile, sizeof(g_temp_delete_dir));
		g_temp_delete_dir[sizeof(g_temp_delete_dir)-1] = '\0';

		printk("%s:<virtual> %s %s -> 0x%08X\n", __func__, oldname, newfile, result);

		return 0;
	}

	if(strlen(oldname) > 4 && 0 == strncmp(oldname+4, "/PSP/GAME/_DEL_", sizeof("/PSP/GAME/_DEL_")-1)) {
		const char *ext;

		ext = strrchr(oldname, '/');

		if (ext != NULL) {
			ext++;

			if(0 == strcmp(ext, "EBOOT.PBP")) {
				result = 0;
				printk("%s:<virtual> %s %s -> 0x%08X\n", __func__, oldname, newfile, result);

				return 0;
			}
		}
	}

	result = sceIoRename(oldname, newfile);
	printk("%s: %s %s -> 0x%08X\n", __func__, oldname, newfile, result);

	return result;
}
