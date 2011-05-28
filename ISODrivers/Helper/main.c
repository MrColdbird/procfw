/*
 * This file is part of PRO CFW.

 * PRO CFW is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * PRO CFW is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with PRO CFW. If not, see <http://www.gnu.org/licenses/ .
 */

/* 
 * An ISO driver test helper plugin
 * Original author: Dark_Alex?
 * From: http://www.pspstation.org/programacion-psp-74/source-medidor-velocidad-ms-dark_alex-5053/
 */
#include <pspsdk.h>
#include <pspkernel.h>
#include <pspctrl.h>

#include <systemctrl.h>
#include <stdio.h>
#include <string.h>

void PatchSyscall(u32 addr, u32 newaddr)
{
	sctrlHENPatchSyscall((void*)addr, (void*)newaddr);
}

PSP_MODULE_INFO("test", 0x1006, 1, 0);

char *slog ;
char *klog;

int (* umd_devctl)(PspIoDrvFileArg *arg, const char *devname, unsigned int cmd, void *indata, int inlen, void *outdata, int outlen);

int ProcessDevctlRead(void *outdata, int size, u32 *indata)
{
   int datasize = indata[4]; // 0x10
   int lba = indata[2]; // 0x08
   int dataoffset = indata[6]; // 0x18
   
   int offset;
   
   if (size < datasize)
      return 0x80010069;

   if (dataoffset == 0)
   {
      offset = lba*0x800;      
   }

   else if (indata[5] != 0)
   {
      offset = (lba*0x800)-dataoffset+0x800;      
   }

   else if (indata[7] == 0)
   {
      offset = (lba*0x800)+dataoffset;      
   }
   else
   {
      offset = (lba*0x800)-dataoffset+0x800;
   }

   sprintf(klog, "umd_devctl_read offset=0x%08x len=0x%08X", offset, datasize);
   klog += strlen(klog);
   return 0;
}


int umd_devctl_patched(PspIoDrvFileArg *arg, const char *devname, unsigned int cmd, void *indata, int inlen, void *outdata, int outlen)
{
#if 0
   if (cmd == 0x01e380c0 || cmd == 0x01f200a1 || cmd == 0x01f200a2)
   {
      ProcessDevctlRead(outdata, outlen, indata);
      
      u32 tm = sceKernelGetSystemTimeLow();
      int res = umd_devctl(arg, devname, cmd, indata, inlen, outdata, outlen);
      sprintf(klog, " (time = %d)\n", (sceKernelGetSystemTimeLow() - tm) / 1000);
      klog += strlen(klog);
      
      return res;
   }
#endif

   int res;

   if(cmd == 0x01F300A7) {
	   sprintf(klog, "devctl cmd=0x%08X iolen=%d/%d idata=0x%08X", cmd, inlen, outlen, *(u32*)indata);
	   klog += strlen(klog);
	   u32 tm = sceKernelGetSystemTimeLow();
	   res = umd_devctl(arg, devname, cmd, indata, inlen, outdata, outlen);
	   sprintf(klog, "-> 0x%08X (time = %d)\n", res, (sceKernelGetSystemTimeLow() - tm) / 1000);
	   klog += strlen(klog);
   } else {
	  res = umd_devctl(arg, devname, cmd, indata, inlen, outdata, outlen);
   }
   
   return res;
}

int sceCtrlReadBufferPositivePatched(SceCtrlData *pad_data, int count);
int sceCtrlPeekBufferPositivePatched(SceCtrlData *pad_data, int count);

void WriteLog()
{
   PspIoDrv *drv = sctrlHENFindDriver("umd");
   drv->funcs->IoDevctl = umd_devctl;
   
   PatchSyscall((u32)sceCtrlReadBufferPositivePatched, sctrlHENFindFunction("sceController_Service", "sceCtrl", 0x1F803938));
   PatchSyscall((u32)sceCtrlPeekBufferPositivePatched, sctrlHENFindFunction("sceController_Service", "sceCtrl", 0x3A622550));
   
   sceKernelDcacheWritebackAll();
   sceKernelIcacheClearAll();
   
   SceUID fd = sceIoOpen("ms0:/drivers.txt", PSP_O_WRONLY | PSP_O_CREAT | PSP_O_TRUNC, 0777);
      
   while (!(*slog)) slog++;
   sceIoWrite(fd, slog, klog-slog);
   sceIoClose(fd);
}

int sceCtrlReadBufferPositivePatched(SceCtrlData *pad_data, int count)
{
   int res = sceCtrlReadBufferPositive(pad_data, count);
   
   int k1 = pspSdkSetK1(0);
   
   if ((pad_data->Buttons & PSP_CTRL_LTRIGGER) && (pad_data->Buttons & PSP_CTRL_RTRIGGER))
   {   
      WriteLog();
   }
   
   pspSdkSetK1(k1);
   
   return res;
}

int sceCtrlPeekBufferPositivePatched(SceCtrlData *pad_data, int count)
{
   int res = sceCtrlPeekBufferPositive(pad_data, count);
   
   int k1 = pspSdkSetK1(0);
   
   if ((pad_data->Buttons & PSP_CTRL_LTRIGGER) && (pad_data->Buttons & PSP_CTRL_RTRIGGER))
   {   
      WriteLog();
   }
   
   pspSdkSetK1(k1);
   
   return res;
}

int module_start(SceSize args, void *argp)
{
   slog = (char *)0x8A000000;
   strcpy(slog, "");
   klog = slog;
   
   PspIoDrv *drv = sctrlHENFindDriver("umd");
   umd_devctl = drv->funcs->IoDevctl;
   drv->funcs->IoDevctl = umd_devctl_patched;   
   
   PatchSyscall(sctrlHENFindFunction("sceController_Service", "sceCtrl", 0x1F803938), (u32)sceCtrlReadBufferPositivePatched);
   PatchSyscall((u32)sctrlHENFindFunction("sceController_Service", "sceCtrl", 0x3A622550), (u32)sceCtrlPeekBufferPositivePatched);
   
   sceKernelDcacheWritebackAll();
   sceKernelIcacheClearAll();
   
   return 0;
}
