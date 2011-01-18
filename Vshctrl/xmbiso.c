#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <pspsdk.h>
#include "xmbiso.h"
#include "systemctrl.h"
#include "systemctrl_se.h"
#include "printk.h"
#include "utils.h"

//game folder descriptor
SceUID gamedfd = -1;

//iso folder descriptor
SceUID isodfd = -1;

//iso folder path
char * isopath = "ms0:/ISO";

//iso counter
u32 isocounter = 0;

//iso loader path (this should be on flash, obviously)
char * isoloader = "ms0:/PSP/SYSTEM/PROMETHEUS/EBOOT.PBP";

//iso mode (m33 mode, this requires a proper vsh-satellite)
int isomode = 1;

//open directory
SceUID gamedopen(const char * dirname)
{
	//forward to firmware
	SceUID result = sceIoDopen(dirname);

	//game folder descriptor
	if (result >= 0 && strlen(dirname) == 13 && strcmp(dirname + 2, "0:/PSP/GAME") == 0) {
		//save descriptor
		gamedfd = result;

		//create iso path from game folder
		strcpy(isopath, dirname);
		strcpy(isopath + 5, "ISO");

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
		if (strcmp(ext, "iso") == 0 || strcmp(ext, "ISO") == 0 || strcmp(ext, "cso") == 0 || strcmp(ext, "CSO") == 0) {
			//valid iso detected (more checks can be added here lateron)
			result = 1;
		}
	}

	//return result
	return result;
}

//inject iso directory
int injectISO(SceIoDirent * dir, int loadexecstage)
{
	//result
	int result = 0;

	//get kernel permission
	u32 k1 = pspSdkSetK1(0);

	//iso folder exists
	if (isodfd >= 0) {
		//iso found flag
		int isofound = 0;

		//search for iso
		while (!isofound) {
			//memset dir memory (sony io crashes otherwise)
			memset(dir, 0, sizeof(SceIoDirent));

			//read next file
			if (sceIoDread(isodfd, dir) > 0) {
				//ignore incompatible entries
				if (dir->d_stat.st_mode != FIO_S_IFDIR && isISO(dir))
					isofound = 1;
			}

			//no more files
			else
				break;
		}

		//found iso
		if (isofound) {
			//patch directory entry data
			if (!loadexecstage) {
				dir->d_stat.st_mode = 0x11FF;
				dir->d_stat.st_attr = 0x10;
				sprintf(dir->d_name, "ISOGAME%08X", isocounter);
			}

			//increase iso counter
			isocounter++;

			//success result
			result = 1;
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
	if (fd == gamedfd && result == 0) result = injectISO(dir, 0);

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
	SceUID result = sceIoOpen(file, flags, mode);

	//virtual iso eboot detected
	if (result == 0x80010002 && strlen(file) == 39 && strncmp(file + 14, "ISOGAME", 7) == 0 && strcmp(file + strlen(file) - 9, "EBOOT.PBP") == 0) {
		//return isoloader descriptor
		u32 k1 = pspSdkSetK1(0);
		strncpy(isoloader, file, 2);
		result = sceIoOpen(isoloader, flags, mode);
		pspSdkSetK1(k1);
	}

	//log call
	//printk("sceIoOpen(%s, %08X, %08X) -> %08X\n", file, flags, mode, result);

	//return result
	return result;
}

//read file
int gameread(SceUID fd, void * data, SceSize size)
{
	//forward to firmware
	int result = sceIoRead(fd, data, size);

	//kernel user permission problem
	if (result == 0x800200D1) {
		//fake kernel permission and retry
		u32 k1 = pspSdkSetK1(0);
		result = sceIoRead(fd, data, size);
		pspSdkSetK1(k1);
	}

	//log call
	//printk("sceIoRead(%08X, %p, %08X) -> %08X\n", fd, data, size, result);

	//return result
	return result;
}

//close file
int gameclose(SceUID fd)
{
	//forward to firmware
	int result = sceIoClose(fd);

	//kernel user permission problem
	if (result == 0x800200D1) {
		//fake kernel permission and retry
		u32 k1 = pspSdkSetK1(0);
		result = sceIoClose(fd);
		pspSdkSetK1(k1);
	}

	//log call
	//printk("sceIoClose(%08X) -> %08X\n", fd, result);

	//return result
	return result;
}

//seek file
SceOff gamelseek(SceUID fd, SceOff offset, int whence)
{
	//forward to firmware
	u32 k1 = pspSdkSetK1(0);
	SceOff result = sceIoLseek(fd, offset, whence);
	pspSdkSetK1(k1);

	//log call
	//printk("sceIoLseek(%08X, %016X, %08X) -> %016X\n", fd, offset, whence, result);

	//return result
	return result;
}

//get file status
int gamegetstat(const char * file, SceIoStat * stat)
{
	//forward to firmware
	int result = sceIoGetstat(file, stat);

	//virtual iso eboot detected
	if (result == 0x80010002 && strlen(file) == 39 && strncmp(file + 14, "ISOGAME", 7) == 0 && strcmp(file + strlen(file) - 9, "EBOOT.PBP") == 0) {
		//return isoloader descriptor
		u32 k1 = pspSdkSetK1(0);
		strncpy(isoloader, file, 2);
		result = sceIoGetstat(isoloader, stat);
		pspSdkSetK1(k1);
	}

	//log call
	//printk("sceIoGetstat(%s, %p) -> %08X\n", file, stat, result);

	//return result
	return result;
}

//remove file
int gameremove(const char * file)
{
	//forward to firmware
	int result = sceIoRemove(file);

	//log call
	printk("sceIoRemove(%s) -> %08X\n", file, result);

	//return result
	return result;
}

//remove folder
int gamermdir(const char * path)
{
	//forward to firmware
	int result = sceIoRmdir(path);

	//log call
	printk("sceIoRmdir(%s) -> %08X\n", path, result);

	//return result
	return result;
}

//load and execute file
int gameloadexec(char * file, struct SceKernelLoadExecVSHParam * param)
{
	//result
	int result = 0;

	//virtual iso eboot detected
	if (strlen(file) == 39 && strncmp(file + 14, "ISOGAME", 7) == 0 && strcmp(file + strlen(file) - 9, "EBOOT.PBP") == 0) {
		//cut iso index string
		char * strisoindex = file + 21;
		file[29] = 0;

		//parse iso index string
		unsigned long isoindex = strtoul(strisoindex, NULL, 16);

		//aquire kernel permission
		u32 k1 = pspSdkSetK1(0);

		//directory entry data
		SceIoDirent dir;

		//iso fetching result
		int isoexists = 0;

		//switch storage device
		strncpy(isopath, file, 2);

		//open iso directory
		isodfd = sceIoDopen(isopath);
		if (isodfd >= 0) {
			//get iso file
			int i = 0; for(; i <= isoindex; i++) isoexists = injectISO(&dir, 1);

			//close iso directory
			sceIoDclose(isodfd);

			//found iso
			if (isoexists) {
				//plenty of memory in dir-struct, using as path storage
				char * gameiso = dir.d_name + strlen(dir.d_name) + 1;

				//create iso path
				sprintf(gameiso, "%s/%s", isopath, dir.d_name);

				//set iso file for reboot
				sctrlSESetUmdFile(gameiso);

				//set iso mode for reboot
				sctrlSESetBootConfFileIndex(isomode);

				//full memory doesn't hurt on isos
				sctrlHENSetMemory(48, 0);

				//reset and configure reboot parameter
				memset(param, 0, sizeof(struct SceKernelLoadExecVSHParam));
				param->size = sizeof(struct SceKernelLoadExecVSHParam);
				param->argp = "disc0:/PSP_GAME/SYSDIR/EBOOT.BIN";
				param->args = strlen(param->argp) + 1;
				param->key = "game";

				//fix apitypes
				int apitype = 0x120;
				if (sceKernelGetModel() == PSP_GO) apitype = 0x125;

				//start game image
				return sctrlKernelLoadExecVSHWithApitype(apitype, param->argp, param);
			}
		}

		//restore user permission
		pspSdkSetK1(k1);
	}

	//forward to ms0 handler
	if(strncmp(file, "ms", 2) == 0) result = sctrlKernelLoadExecVSHMs2(file, param);

	//forward to ef0 handler
	else result = sctrlKernelLoadExecVSHEf2(file, param);

	//return result
	return result;
}
