/*************************************/
/*									*/
/*************************************/

#include <pspkernel.h>
#include <pspctrl.h>
#include <pspdebug.h>
#include <psppower.h>
#include <pspsysmem_kernel.h>

#include <psploadexec_kernel.h>
#include <kubridge.h>
#include <pspvshbridge.h>

#include <systemctrl.h>
#include <systemctrl_se.h>

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "pspipl_update.h"

#include "../cfw_modules.h"
#include "../../CIPL/ipl_block_large.h"
#include "../../CIPL/ipl_block_01g.h"

#include "../modules/ipl_update_packed.h"

#define printf pspDebugScreenPrintf

PSP_MODULE_INFO("plutonium_updater", 0x800, 0, 1);

extern unsigned int size_pupd;
extern unsigned char pupd[];

extern unsigned int size_u235;
extern unsigned char u235[];

extern unsigned int size_downloader;
extern unsigned char downloader[];


#if _PSP_FW_VERSION == 639
u8 ipl_hash639_01g[16] = { 0xFC, 0x4C, 0xDF, 0xA6, 0x5D, 0x87, 0x09, 0xDA, 0xBD, 0x07, 0x36, 0x03, 0x3D, 0x1B, 0xC1, 0xA3 };
u8 ipl_hash639_02g[16] = { 0xE7, 0x20, 0xDE, 0x41, 0xB3, 0x1B, 0xFA, 0x19, 0xBA, 0x1A, 0x86, 0x92, 0x9D, 0xBC, 0x0C, 0xBC };
#define TARGET_PBP "639.PBP"
#define DEVKIT_VER 0x06030910
#endif

int sceDisplaySetHoldMode(int a0);

int WriteFile(char *file, void *buf, int size)
{
	SceUID fd = sceIoOpen(file, PSP_O_WRONLY | PSP_O_CREAT | PSP_O_TRUNC, 0777);
	if (fd < 0) return -1;

	int written = sceIoWrite(fd, buf, size);
	if (sceIoClose(fd) < 0) return -1;

	return written;
}

void ErrorExit(int milisecs, char *fmt, ...)
{
	va_list list;
	char msg[256];

	va_start(list, fmt);
	vsprintf(msg, fmt, list);
	va_end(list);

	pspDebugScreenPrintf(msg);

	sceKernelDelayThread(milisecs*1000);
	sceKernelExitGame();
}

int StopUnloadModule(SceUID id)
{
	SceUID r = sceKernelStopModule(id ,0 ,NULL , NULL, NULL);
	if( r < 0 ) return r;

	return sceKernelUnloadModule(id);
}

int LoadStartModuleBuffer(char *buff,int size , int flag)
{
	SceUID mod = vshKernelLoadModuleBufferVSH(size ,buff, 0, NULL);
	if (mod < 0) return mod;

	return sceKernelStartModule(mod, (flag == 0)? 0 : 4 , (void *)flag , NULL, NULL);
}

void Agreement()
{
	SceCtrlData pad;
	while (1)
	{
		sceCtrlReadBufferPositive(&pad, 1);

		if (pad.Buttons & PSP_CTRL_CROSS) {
			return;			
		}
		else if (pad.Buttons & PSP_CTRL_RTRIGGER) {
			ErrorExit(5000,"Update canceled by user.\n");
		}
		sceKernelDelayThread(10000);
	}
}

int ByPass(u32 mask )
{
	SceCtrlData pad;
	sceCtrlReadBufferPositive(&pad, 1);

	if ((pad.Buttons & mask) == mask) 
		return 1;

	return 0;
}

int GetMD5Buffer(u8 *buf, u32 size, u8 *md5)
{
	u8 digest[16];
	sceKernelUtilsMd5Digest( buf , size , digest );
	memcpy( md5 , digest , 0x10 );
	return 0;
}

u8 buff[0x1000];

int CheckMD5(char *path ,u8 *hash )
{
	SceUID fd;
	u8 digest[16];
	SceKernelUtilsMd5Context ctx;
	int size;

	printf("Verifying %s... ",path);

	if((fd = sceIoOpen(path , PSP_O_RDONLY, 0777)) < 0) {
		printf("Cannot open file (0x%08X).\n",fd);
		return -1;
	}

	sceKernelUtilsMd5BlockInit(&ctx);

	while((size = sceIoRead(fd , buff , 0x1000)) > 0) {
		sceKernelUtilsMd5BlockUpdate(&ctx, buff , size);
	}

	sceIoClose(fd);
	sceKernelUtilsMd5BlockResult(&ctx, digest);

	if ( memcmp( hash , digest , 16) ) {
		printf("Incorrect file.\n");
		return -1;
	}

	printf("OK\n");
	return 0;
}

void flash_ipl(u8 *nand_buff , int size)
{
	printf("Flashing IPL...");

	if(pspIplUpdateClearIpl() < 0)
		ErrorExit(5000,"Failed to clear ipl!\n");

	if (pspIplUpdateSetIpl( nand_buff , size ) < 0)
		ErrorExit(5000,"Failed to write ipl!\n");

	printf("Done.\n");
}

int CheckIPL( int model )
{
	u8 nand_buff[size_ipl_block_large];//0x24000
	u8 md5_buff[16];
	u8 md5_buff2[16];
	u32 offset = 0;
	const u8* ipl_hash;

	if( model == 0)
	{
		memcpy( ipl_block_large , ipl_block_01g , 0x4000 );
		ipl_hash = ipl_hash639_01g;
	}
	else
	{
		ipl_hash = ipl_hash639_02g;
	}

	printf("Verifying installed ipl ... ");

	int size = pspIplUpdateGetIpl( nand_buff  );		
	if(size < 0)
		ErrorExit(5000,"Failed to get ipl!\n");

	if( size != (size_ipl_block_large - 0x4000))
	{
		GetMD5Buffer(  ipl_block_large  , 0x4000 , md5_buff );
		GetMD5Buffer( nand_buff , 0x4000 , md5_buff2);

		if( memcmp( md5_buff , md5_buff2 , 16) == 0)
		{
			//already installed
			printf("Already installed \n");
			return 0;
		}

		offset = 0x4000;
	}
	
	GetMD5Buffer( nand_buff + offset , size - offset , md5_buff );

	if(memcmp( md5_buff , ipl_hash , 16))
		ErrorExit(5000,"ERROR ipl is not real!\n");

	memcpy( ipl_block_large + 0x4000 , nand_buff + offset , size - offset );

	printf("\n");
	flash_ipl( ipl_block_large , size_ipl_block_large );
	return 0;
}
int main(int argc, char** argv)
{
	u32 mod;
	SceIoStat fstat;
	struct SceKernelLoadExecVSHParam param;
	int i;

	pspDebugScreenInit();

	int model = kuKernelGetModel();
	if( !( model == 0 || model == 1) )
	{
		ErrorExit(5000,"This installer does not support this model.\n");
	}

	sceIoChdir("ms0:/PSP/GAME/UPDATE");

	if(sceKernelDevkitVersion() != DEVKIT_VER )
	{
		printf("Unpacking... ");
		WriteFile("PUPD.PBP", pupd, size_pupd );
		WriteFile("u235.prx",  u235 , size_u235 );
		printf(" done.\n");

		memset(&fstat, 0, sizeof(SceIoStat));
		if(sceIoGetstat( TARGET_PBP ,&fstat)<0)
		{			
			//dl				
			printf("\n" TARGET_PBP " doesn't exist.\nDo you want to download it from internet? (x=yes, R=no).\n");
			Agreement();

			printf("Unpacking... ");
			WriteFile("DL.PBP",  downloader , size_downloader );
			printf(" done.\n");

			memset(&param, 0, sizeof(param));	
			param.size=sizeof(param);
			param.args=28;
			param.argp="ms0:/PSP/GAME/UPDATE/DL.PBP";
			param.key="game";

			printf("\nPlease wait...\n");

			sceDisplaySetHoldMode(1);
			sctrlKernelLoadExecVSHMs2("ms0:/PSP/GAME/UPDATE/DL.PBP",&param);
			sceKernelSleepThread();
			return 0;
		}

		memset(&param, 0, sizeof(param));

		param.size=sizeof(param);
		param.args=strlen("ms0:/PSP/GAME/UPDATE/PUPD.PBP")+1;
		param.argp="ms0:/PSP/GAME/UPDATE/PUPD.PBP";
		param.key="updater";

		printf("\nPlease wait...\n");

		sceDisplaySetHoldMode(1);
		sctrlKernelLoadExecVSHMs1("ms0:/PSP/GAME/UPDATE/PUPD.PBP",&param);
		sceKernelSleepThread();
		return 0;
	}

	mod = LoadStartModuleBuffer( (char *)ipl_update, size_ipl_update, 0 );
	if(mod < 0) {
		ErrorExit(6000, "\nError 0x%08X Loading/Starting ipl_update.\n", mod);
	}


	/*
	u32 se_ver = sctrlSEGetVersion();
	if( se_ver >= 0x20001)
		ErrorExit(5000, "This update or a higher one was already applied.\n");
	*/

	printf("\nCFW 6.39PRO installer for Hackable PSP .\n\n");
/*
	printf("\nCFW 6.39 installer for Hackable PSP by neur0n.\n\n");
	printf("Changes:\n\n");
	
	switch( se_ver & 0xFF ) {
#include "../change_log.h"
	}
	printf("\n");


	Module *modules = NULL;
	if( model == 0 )
		modules = (Module *)modules_01g;
	else
		modules = (Module *)modules_02g;
*/
	printf("Press X to start the update, R to exit.\n\n");
	Agreement();

	if ( ( mod =CheckIPL( model )) < 0) {
		ErrorExit(6000, "\nError 0x%08X create custom ipl.\n", mod);
	}

	if(sceIoUnassign("flash0:") < 0)
		ErrorExit(5000,"Error in unassign.\n");

	if (sceIoAssign("flash0:", "lflash0:0,0", "flashfat0:", IOASSIGN_RDWR, NULL, 0) < 0)
		ErrorExit(5000,"Error in assign.\n");

//	sceIoRemove("ms0:/PSP/SYSTEM/isocaches.bin");
//	sceIoRemove("flash1:/config.me");

	for(i = 0; i < COMMON_MODULES_COUNT; i++) {
		printf("Flashing %s (%d)... ",common_modules[i].dst, *common_modules[i].size);
		WriteFile(common_modules[i].dst, common_modules[i].buffer, *common_modules[i].size);
		printf("OK\n");
	}


	ErrorExit(6000,"\nUpdate complete. Restarting in 6 seconds...\n");
	return 0;
}

