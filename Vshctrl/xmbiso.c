#include "xmbiso.h"
#include "systemctrl.h"
#include "systemctrl_se.h"
#include "isoreader.h"
#include "printk.h"
#include "utils.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <pspsdk.h>

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

//iso mode (m33 mode, this requires a proper vsh-satellite)
int isomode = 1;

//temporary filename
char * temppath = "ms0:/C01DB15D.bin";

//memory for param.sfo
u8 titleinfo[1024];

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
		if (strcmp(ext, "iso") == 0 || strcmp(ext, "ISO") == 0 /* || strcmp(ext, "cso") == 0 || strcmp(ext, "CSO") == 0 */) {
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

//make param.sfo bootable
void makebootable(u8 * param, u32 size)
{
	//number of items
	u32 count = *(u32 *)(param + 16);

	//key table pointer
	u8 * keys = param + *(u32 *)(param + 8);

	//value table pointer
	u8 * values = param + *(u32 *)(param + 12);

	//index table pointer
	u8 * indices = param + 20;

	//iterate items
	int i = 0; for(; i < count; i++) {
		//grab index
		u8 * index = indices + 16 * i;

		//grab key
		char * key = (char *)keys + *(u16 *)(index);

		//we found the category key
		if(strcmp(key, "CATEGORY") == 0) {
			//grab value
			char * value = (char *)values + *(u32 *)(index + 12);

			//override value
			strncpy(value, "MG", 2);

			//stop hack
			break;
		}
	}
}

//create and open temporary eboot
SceUID opentemp(const char * file, int flags, SceMode mode)
{
	//result
	SceUID result = -1;

	//aquire kernel permission
	u32 k1 = pspSdkSetK1(0);

	//translate iso name
	char * isoname = pathtranslator((char *)file);

	//found iso
	if (isoname) {
		//fix temporary path
		strncpy(temppath, file, 2);

		//mount iso file in isoreader
		isoSetFile(isoname);

		//create temporary file
		result = sceIoOpen(temppath, PSP_O_WRONLY | PSP_O_TRUNC | PSP_O_CREAT, 0777);
		if (result >= 0) {
			//write pbp magic
			u32 temp = 0x50425000;
			sceIoWrite(result, &temp, sizeof(temp));

			//write pbp version
			temp = 0x10000;
			sceIoWrite(result, &temp, sizeof(temp));

			//write param.sfo offset
			temp = 40;
			sceIoWrite(result, &temp, sizeof(temp));

			//get param.sfo information
			u32 paramsize = 0;
			u32 paramlba = isoGetFileInfo("PARAM.SFO", &paramsize);

			//move offset
			temp += paramsize;

			//dummy unwanted eboot files
			int i = 0; for(; i < 7; i++) sceIoWrite(result, &temp, sizeof(temp));

			//flush param.sfo
			if (paramlba) {
				//read param.sfo
				isoReadRawData(titleinfo, paramlba, 0, paramsize);

				//patch game to be memory stick bootable
				makebootable(titleinfo, paramsize);

				//write param.sfo
				sceIoWrite(result, titleinfo, paramsize);
			}

			//close temporary file
			sceIoClose(result);
		}


		//debugging: some japanese umd game param.sfo are fucked up...
		//way of the samurai is one of those it seems.
		//if(strstr(isoname, "Samurai") != NULL) {
		//	sceIoRename(temppath, "ef0:/samurai.pbp");
		//}

		//open temporary eboot
		result = sceIoOpen(temppath, flags, mode);
	}

	//restore user permission
	pspSdkSetK1(k1);

	//return descriptor
	return result;
}

//open file
SceUID gameopen(const char * file, int flags, SceMode mode)
{
	//forward to firmware
	SceUID result = sceIoOpen(file, flags, mode);

	//virtual iso eboot detected
	if (result == 0x80010002 && ISOEBOOT(file)) {
		//open temporary iso eboot
		result = opentemp(file, flags, mode);
	}

	//return result
	return result;
}

//read file
int gameread(SceUID fd, void * data, SceSize size)
{
	//aquire kernel permission
	u32 k1 = pspSdkSetK1(0);

	//forward to firmware
	int result = sceIoRead(fd, data, size);

	//restore user permission
	pspSdkSetK1(k1);

	//return result
	return result;
}

//close file
int gameclose(SceUID fd)
{
	//aquire kernel permission
	u32 k1 = pspSdkSetK1(0);

	//forward to firmware
	int result = sceIoClose(fd);

	//delete temporary eboot
	if (fd > 0x1000) sceIoRemove(temppath);

	//restore user permission
	pspSdkSetK1(k1);

	//return result
	return result;
}

//seek file
SceOff gamelseek(SceUID fd, SceOff offset, int whence)
{
	//result
	SceOff result = 0;

	//aquire kernel permission
	u32 k1 = pspSdkSetK1(0);

	//forward to firmware
	result = sceIoLseek(fd, offset, whence);

	//restore user permission
	pspSdkSetK1(k1);

	//return result
	return result;
}

//get file status
int gamegetstat(const char * file, SceIoStat * stat)
{
	//forward to firmware
	int result = sceIoGetstat(file, stat);

	//virtual iso eboot detected
	if (result == 0x80010002 && ISOEBOOT(file)) {
		//translate path
		char * isopath = pathtranslator((char *)file);

		//found iso
		if (isopath) {
			//return iso status
			u32 k1 = pspSdkSetK1(0);
			result = sceIoGetstat(isopath, stat);
			pspSdkSetK1(k1);
		}
	}

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
	if (ISOEBOOT(file)) {
		//translate iso path
		char * gameiso = pathtranslator(file);

		//found iso
		if (gameiso) {
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

	//forward to ms0 handler
	if(strncmp(file, "ms", 2) == 0) result = sctrlKernelLoadExecVSHMs2(file, param);

	//forward to ef0 handler
	else result = sctrlKernelLoadExecVSHEf2(file, param);

	//return result
	return result;
}

//translate virtual eboot to iso path
char * pathtranslator(char * file)
{
	//result
	char * result = NULL;

	//internal buffer
	static char buffer[256];

	//fix iso path
	strncpy(isopath, file, 2);

	//cut iso index string
	strcpy(buffer, file);
	char * strisoindex = buffer + 21;
	buffer[29] = 0;

	//parse iso index string
	unsigned long isoindex = strtoul(strisoindex, NULL, 16);

	//aquire kernel permission
	u32 k1 = pspSdkSetK1(0);

	//open iso directory
	SceUID isodfd = sceIoDopen(isopath);
	if (isodfd >= 0) {
		//found flag
		int isoexists = 0;

		//directory entry
		SceIoDirent dir;

		//get iso file
		int i = 0; for(; i <= isoindex; i++) isoexists = injectISO(&dir, isodfd, 1);

		//close iso directory
		sceIoDclose(isodfd);

		//found iso
		if (isoexists) {
			//create iso path
			sprintf(buffer, "%s/%s", isopath, dir.d_name);

			//set as result
			result = buffer;
		}
	}

	//restore user permission
	pspSdkSetK1(k1);

	//return result
	return result;
}
