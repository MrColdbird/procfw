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

//virtual eboot detect macro
#define ISOEBOOT(file) (strlen(file) == 39 && strncmp(file + 14, "ISOGAME", 7) == 0 && strcmp(file + strlen(file) - 9, "EBOOT.PBP") == 0)

//game folder descriptor
SceUID gamedfd = -1;

//iso folder descriptor
SceUID isodfd = -1;

//iso folder path
char * isopath = "ms0:/ISO";

//iso counter
u32 isocounter = 0;

//open directory
SceUID gamedopen(const char * dirname)
{
	//forward to firmware
	SceUID result = sceIoDopen(dirname);

	//game folder descriptor
	if (result >= 0 && strlen(dirname) == 13 && strcmp(dirname + 2, "0:/PSP/GAME") == 0) {
		//save descriptor
		gamedfd = result;

		//fix iso path
		strncpy(isopath, dirname, 2);

		//open iso directory
		u32 k1 = pspSdkSetK1(0);
		isodfd = sceIoDopen(isopath);
		pspSdkSetK1(k1);
	}

	//return result
	return result;
}

//valid iso check
int isISO(SceIoDirent * dir)
{
	//result
	int result = 0;

	//grab extension
	char * ext = dir->d_name + strlen(dir->d_name) - 3;

	//filename length check
	if (ext > dir->d_name) {
		//check extension
		if (stricmp(ext, "iso") == 0 /* || strcmp(ext, "cso") == 0 || strcmp(ext, "CSO") == 0 */) {
			//valid iso detected (more checks can be added here lateron)
			result = 1;
		}
	}

	//return result
	return result;
}

//inject iso directory
int injectISO(SceIoDirent * dir, SceUID isodfd, int loadexecstage)
{
	//result
	int result = 0;

	//get kernel permission
	u32 k1 = pspSdkSetK1(0);

	//iso folder exists
	if (isodfd >= 0) {
		//search for iso
		while (!result) {
			//memset dir memory (sony io crashes otherwise)
			memset(dir, 0, sizeof(SceIoDirent));

			//read next file
			if (sceIoDread(isodfd, dir) > 0) {
				//ignore incompatible entries
				if (dir->d_stat.st_mode != FIO_S_IFDIR && isISO(dir))
					result = 1;
			}

			//no more files
			else
				break;
		}

		//patch directory entry data
		if (result && !loadexecstage) {
			dir->d_stat.st_mode = 0x11FF;
			dir->d_stat.st_attr = 0x10;
			sprintf(dir->d_name, "ISOGAME%08X", isocounter);

			//increase iso counter
			isocounter++;
		}
	}

	//restore user permission
	pspSdkSetK1(k1);

	//return result
	return result;
}

//read directory
int gamedread(SceUID fd, SceIoDirent * dir)
{
	//forward to firmware
	int result = sceIoDread(fd, dir);

	//inject fake iso folder
	if (fd == gamedfd && result == 0) result = injectISO(dir, isodfd, 0);

	//return result
	return result;
}

//directory descriptor closer
int gamedclose(SceUID fd)
{
	//forward to firmware
	int result = sceIoDclose(fd);

	//game folder descriptor
	if (fd == gamedfd) {
		//erase game folder reference
		gamedfd = -1;

		//close iso directory
		sceIoDclose(isodfd);

		//erase iso folder reference
		isodfd = -1;

		//reset iso counter
		isocounter = 0;
	}

	//return result
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

	//return result
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

	//return result
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
	
	//return result
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

	
	//return result
	return result;
}

//remove file
int gameremove(const char * file)
{
	int result;
   
	if (ISOEBOOT(file)) {
		printk("%s: %s", __func__, file);
		u32 k1 = pspSdkSetK1(0);
		result = vpbp_remove(file);
		pspSdkSetK1(k1);
		printk(" -> 0x%08X\n", result);
	} else {
		result = sceIoRemove(file);
	}

	return result;
}

//remove folder
int gamermdir(const char * path)
{
	//forward to firmware
	int result = sceIoRmdir(path);

	printk("%s: %s 0x%08X\n", __func__, path, result);

	//return result
	return result;
}

//load and execute file
int gameloadexec(char * file, struct SceKernelLoadExecVSHParam * param)
{
	//result
	int result = 0;

	printk("%s: %s\n", __func__, file);
	
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

	//return result
	return result;
}
