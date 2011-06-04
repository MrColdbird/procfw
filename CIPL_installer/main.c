/********************************
	ipl Flasher 


*********************************/
#include <pspsdk.h>
#include <pspkernel.h>
#include <pspctrl.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <kubridge.h>

#include "pspipl_update.h"
#include "../CIPL/ipl_block_large.h"
#include "../CIPL/ipl_block_01g.h"

PSP_MODULE_INFO("IPLFlasher", 0x0800, 1, 0); 
PSP_MAIN_THREAD_ATTR(PSP_THREAD_ATTR_VSH);

#define printf pspDebugScreenPrintf
#define RED 0x111FFF
#define WHITE 0xFFFFF1
#define GREEN 0x0000FF00

u32 sceSysregGetTachyonVersion(void);		// 0xE2A5D1EE

char msg[256];
int model;

int ReadFile(char *file, int seek, void *buf, int size)
{
	SceUID fd = sceIoOpen(file, PSP_O_RDONLY, 0);
	if (fd < 0)
		return fd;

	if (seek > 0)
	{
		if (sceIoLseek(fd, seek, PSP_SEEK_SET) != seek)
		{
			sceIoClose(fd);
			return -1;
		}
	}

	int read = sceIoRead(fd, buf, size);
	
	sceIoClose(fd);
	return read;
}


////////////////////////////////////////
void ErrorExit(int milisecs, char *fmt, ...) 
{
	va_list list;
	va_start(list, fmt);
	vsprintf(msg, fmt, list);
	va_end(list);
	printf(msg);
	sceKernelDelayThread(milisecs*1000);
	sceKernelExitGame(); 
}
////////////////////////////////////////
int flash_file(char *file, void *file_name,  int file_size) 
{
	int written;
	SceUID fd;

    printf("Writing File %s.... ", file);
    fd = sceIoOpen(file, PSP_O_WRONLY | PSP_O_CREAT | PSP_O_TRUNC , 511 );
    if(fd < 0)
	{
          pspDebugScreenSetTextColor(RED);
          ErrorExit(5000, "\n\nCannot open file for writing.\n"); 
	}

    written = sceIoWrite(fd, file_name, file_size);
    if(written != file_size) 
	{
		sceIoClose(fd);     
		pspDebugScreenSetTextColor(RED);
		ErrorExit(5000, "\n\nCannot write file.\n"); 
	}
    sceIoClose(fd);
    printf("OK\n\n");
    return 0;
}
////////////////////////////////////////
void remove_files(char *files) {
     printf("Removing File %s....", files);
     sceIoRemove(files);
     printf("OK\n"); }

int Assign()
{
	if (sceIoUnassign("flash0:") < 0)
		return -1;
	
	if (sceIoAssign("flash0:", "lflash0:0,0", "flashfat0:", IOASSIGN_RDWR, NULL, 0) < 0)
		return -1;

	return 0;
}
////////////////////////////////////////

u8 nand_buff[0x40000];

int FileCopy(const char *name , const char * to)
{
	char path_buff[64];
	int size;

	strcpy( path_buff , to );
	strcat( path_buff , name);

	SceUID fd_write = sceIoOpen( path_buff, PSP_O_WRONLY | PSP_O_CREAT | PSP_O_TRUNC, 0777);

//	strcpy( path_buff , "ms0:/PSP/GAME/UPDATE/6xxModules/");
	strcpy( path_buff , name);

	SceUID fd_load = sceIoOpen( path_buff , PSP_O_RDONLY, 0777);

	if( fd_write <0 || fd_load <0)
	{
		sceIoClose(fd_write);
		sceIoClose(fd_load);
		return -1;
	}
	printf("writing %s...", name);

	while((size = sceIoRead(fd_load , nand_buff , 0x30000)) > 0)
	{
		sceIoWrite(fd_write, nand_buff , size);
	}

		sceIoClose(fd_write);
		sceIoClose(fd_load);

		printf("Done.\n");
	return 0;
}


void flash_ipl(int size)
{

	printf("Flashing IPL...");

//	if(ReadFile("ipl_block.bin", 0 , ipl_block_large , 0x4000) < 0)
//		ErrorExit(5000,"Failed to load custom ipl!\n");

	if(pspIplUpdateClearIpl() < 0)
		ErrorExit(5000,"Failed to clear ipl!\n");

	if (pspIplUpdateSetIpl( ipl_block_large , size + 0x4000 ) < 0)
		ErrorExit(5000,"Failed to write ipl!\n");

	printf("Done.\n");

}

int is_ta88v3(void)
{
	u32 model, tachyon;

	tachyon = sceSysregGetTachyonVersion();
	model = kuKernelGetModel();

	if(model == 1 && tachyon == 0x00600000) {
		return 1;
	}

	return 0;
}

int main() 
{
	int devkit, size;
	SceUID kpspident;
	SceUID mod;

	(void)size_ipl_block_large;

	pspDebugScreenInit();
	pspDebugScreenSetTextColor(WHITE);
	devkit = sceKernelDevkitVersion();

	if(devkit != 0x06030910) {
		ErrorExit(5000,"FW ERROR!\n");
	}

	kpspident = pspSdkLoadStartModule("kpspident.prx", PSP_MEMORY_PARTITION_KERNEL);

	if (kpspident < 0) {
		ErrorExit(5000, "kpspident.prx loaded failed\n");
	}

	model = kuKernelGetModel();

	if(!(model == 0 || model == 1) || is_ta88v3()) {
		ErrorExit(5000,"This installer does not support this model.\n");
	}

	if( model == 0 ) {
		memcpy( ipl_block_large , ipl_block_01g, 0x4000);
	}

	//load module
	mod = sceKernelLoadModule("ipl_update.prx", 0, NULL);

	if (mod < 0) {
		ErrorExit(5000,"Could not load ipl_update.prx!\n");
	}

	mod = sceKernelStartModule(mod, 0, NULL, NULL, NULL);

	if (mod < 0) {
		ErrorExit(5000,"Could not start module!\n");
	}

	size = pspIplUpdateGetIpl( ipl_block_large + 0x4000 );

	if(size < 0) {
		ErrorExit(5000,"Failed to get ipl!\n");
	}

	printf("\nCustom ipl Flasher for 6.39.\n\n\n");

	int ipl_type = 0;

	if( size == 0x24000 ) {
		printf("Custom ipl is installed\n");
		size -= 0x4000;
		memmove( ipl_block_large + 0x4000 , ipl_block_large + 0x8000 , size);
		ipl_type = 1;
	} else if( size == 0x20000 ) {
		printf("Raw ipl \n");
	} else {
		printf("ipl size;%08X\n", size);
//		if(( size = ReadFile("raw_ipl.bin", 0 , ipl_block_large + 0x4000 , 0x30000)) < 0)
		ErrorExit(5000,"Unknown ipl!\n");
	}

	printf(" Press X to ");

	if( ipl_type ) {
		printf("Re");
	}

	printf("install CIPL\n");

	if( ipl_type ) {
		printf(" Press O to Erase CIPL and Restore Raw IPL\n");
	}

	printf(" Press R to cansel\n\n");
    
	while (1) {
        SceCtrlData pad;
        sceCtrlReadBufferPositive(&pad, 1);

		if (pad.Buttons & PSP_CTRL_CROSS) {
			flash_ipl( size );

//		if(Assign()<0)
//		ErrorExit(5000,"Error in assign.\n");
//			FileCopy("pspbtjnf_02g.bin" , "flash0:/kd/");
//			FileCopy("vshctrl_02g.prx" , "flash0:/kd/");
//			FileCopy("recovery.prx" , "flash0:/vsh/module/");
//			FileCopy("satelite.prx" , "flash0:/vsh/module/");
			
			break; 
		} else if ( (pad.Buttons & PSP_CTRL_CIRCLE) && ipl_type ) {		
			printf("Flashing IPL...");

			if(pspIplUpdateClearIpl() < 0) {
				ErrorExit(5000,"Failed to clear ipl!\n");
			}

			if (pspIplUpdateSetIpl( ipl_block_large + 0x4000 , size ) < 0) {
				ErrorExit(5000,"Failed to write ipl!\n");
			}

			printf("Done.\n");
			break; 
		} else if (pad.Buttons & PSP_CTRL_RTRIGGER) {
			ErrorExit(2000,"Cancelled by user.\n");
		}

		sceKernelDelayThread(10000);
	}

	ErrorExit(5000,"\nInstall complete. Restarting in 5 seconds...\n");

	return 0;
}
