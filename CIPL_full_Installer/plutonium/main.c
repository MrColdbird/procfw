/*************************************/
/*									*/
/*************************************/

#include <pspkernel.h>
#include <pspctrl.h>
#include <pspdebug.h>
#include <psppower.h>
#include <pspvshbridge.h>

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include <kubridge.h>
#include <systemctrl.h>
#include <systemctrl_se.h>

#define printf pspDebugScreenPrintf

PSP_MODULE_INFO("plutonium_updater", 0x800, 0, 1);

extern int size_plutonium;
extern unsigned char plutonium[];

int PlutoniumGetModel();
int PlutoniumStartUpdater();

#include "../u235/u235_md5.h"

#if _PSP_FW_VERSION == 639
u8 hash639[16] = { 0xCC, 0xCE, 0x1A, 0x0F, 0x3B, 0xA0, 0x8E, 0x22, 0xC2, 0x6E, 0xC5, 0xBC, 0x04, 0x7A, 0x00, 0x63 };
#define TARGET_PBP "639.PBP"
#define REMOVE_ISOCACHE
#endif

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

int LoadStartModuleBuffer(char *buff,int size , void* flag)
{
	SceUID mod = vshKernelLoadModuleBufferVSH(size ,buff, 0, NULL);
	if (mod < 0) return mod;

	return sceKernelStartModule(mod, (flag == NULL)? 0 : 4 , flag , NULL, NULL);
}

void Agreement()
{
	SceCtrlData pad;
	while (1) {
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

u8 buff[0x2000];

int CheckMD5(char *path ,u8 *hash )
{
	SceUID fd;
	u8 digest[16];
	SceKernelUtilsMd5Context ctx;
	int size;

	printf("Verifying %s... ",path);

	if((fd = sceIoOpen(path , PSP_O_RDONLY, 0777)) < 0) {
		ErrorExit(5000,"Cannot open file (0x%08X).\n",fd);
	}

	sceKernelUtilsMd5BlockInit(&ctx);

	while((size = sceIoRead(fd , buff , 0x2000)) > 0) {
		sceKernelUtilsMd5BlockUpdate(&ctx, buff , size);
	}

	sceIoClose(fd);
	sceKernelUtilsMd5BlockResult(&ctx, digest);

	if ( memcmp( hash , digest , 16) ) {
		ErrorExit(5000 ,"Incorrect file.\n");
	}

	printf("OK\n");
	return 0;
}

int backup_act()
{
	sceIoUnassign("flash2:");
	int res = sceIoAssign("flash2:", "lflash0:0,2", "flashfat2:", IOASSIGN_RDWR, NULL, 0);
	if( res >= 0)
	{
		sceIoRemove("act.dat");
		int fd = sceIoOpen("flash2:/act.dat" , PSP_O_RDONLY, 0777);
		if( fd >= 0) {
			int size = sceIoRead(fd , buff , 0x2000);
			sceIoClose(fd);

			if(size > 0) WriteFile("act.dat",  buff ,  size);
		}
		sceIoUnassign("flash2:");
	}

	return 0;
}

int main(int argc, char** argv)
{
	u32 mod;
	pspDebugScreenInit();

	int flag = ByPass(PSP_CTRL_TRIANGLE | PSP_CTRL_LTRIGGER);
		
	sceIoChdir("ms0:/PSP/GAME/UPDATE");

	int model = kuKernelGetModel();
	if( !( model == 1 || model == 0 ) ) {
		ErrorExit(5000,"This program support only 01g or 02g model.\n");
	}

	if(sceKernelDevkitVersion() < 0x04000010) {
		ErrorExit(5000,"This program requires 4.00 M33 or higher.\n");
	}

	if(flag == 0) {
		if(scePowerGetBatteryLifePercent() < 78 )
			ErrorExit(5000 , "Battery has to be at least at 78%%.\n");
	}

	if(!ByPass(PSP_CTRL_SELECT | PSP_CTRL_START)) {
		CheckMD5( TARGET_PBP , (u8 *)hash639);
		CheckMD5("u235.prx",(u8 *)hash235);
	}

	printf("Press X to start the update, R to exit.\n\n");
	Agreement();

	mod = LoadStartModuleBuffer( (char *)plutonium, size_plutonium, &flag );
	if(mod < 0) {
		ErrorExit(6000, "Error 0x%08X loading plutonium.\n", mod);
	}
/*
#ifdef REMOVE_ISOCACHE
	sceIoRemove("ms0:/PSP/SYSTEM/isocaches.bin" );
#endif
*/
	backup_act();

	printf("Starting sce updater. Wait...\n");
	sceKernelDelayThread(2*1000*1000);

	mod = PlutoniumStartUpdater();
	if( mod < 0) {
		ErrorExit(6000, "\nError start plutonium 0x%08X.\n", mod);
	}

	sceKernelExitDeleteThread(0);
	return 0;
}

