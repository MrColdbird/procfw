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

#include <pspkernel.h>
#include <pspsdk.h>
#include <pspctrl.h>
#include <psprtc.h>
#include <pspsysmem_kernel.h>
#include <pspexception.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include "strsafe.h"
#include "printk.h"
#include "systemctrl.h"
#include "utils.h"

#define MAX_BACKTRACE_NUM 10

PSP_MODULE_INFO("exception", 0x1007, 1, 1);
PSP_MAIN_THREAD_ATTR(0);

extern u8 myDebugExceptionHandler;
extern int psplinkPresent(void);

PspDebugErrorHandler curr_handler;
PspDebugRegBlock exception_regs;

static const char *codeTxt[32] = {
	"Interrupt", "TLB modification", "TLB load/inst fetch", "TLB store",
	"Address load/inst fetch", "Address store", "Bus error (instr)",
	"Bus error (data)", "Syscall", "Breakpoint", "Reserved instruction",
	"Coprocessor unusable", "Arithmetic overflow", "Unknown 14",
	"Unknown 15", "Unknown 16", "Unknown 17", "Unknown 18", "Unknown 19",
	"Unknown 20", "Unknown 21", "Unknown 22", "Unknown 23", "Unknown 24",
	"Unknown 25", "Unknown 26", "Unknown 27", "Unknown 28", "Unknown 29",
	"Unknown 31"
};

static const unsigned char regName[32][5] = {
	"zr", "at", "v0", "v1", "a0", "a1", "a2", "a3",
	"t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7",
	"s0", "s1", "s2", "s3", "s4", "s5", "s6", "s7",
	"t8", "t9", "k0", "k1", "gp", "sp", "fp", "ra"
};

static void reboot_vsh_with_error(u32 error)
{
	struct SceKernelLoadExecVSHParam param;	
	u32 vshmain_args[0x20/4];

	memset(&param, 0, sizeof(param));
	memset(vshmain_args, 0, sizeof(vshmain_args));

	vshmain_args[0/4] = 0x0400;
	vshmain_args[4/4] = 0x20;
	vshmain_args[0x14/4] = error;

	param.size = sizeof(param);
	param.args = 0x400;
	param.argp = vshmain_args;
	param.vshmain_args_size = 0x400;
	param.vshmain_args = vshmain_args;
	param.configfile = "/kd/pspbtcnf.txt";

	sctrlKernelExitVSH(&param);
}

void ExceptionHandler(PspDebugRegBlock * regs)
{
	int i, found;
	PspDebugStackTrace traces[MAX_BACKTRACE_NUM];
	char timestr[80];
	pspTime tm;
	SceModule2 *mod;
	const char *modname;
	u32 mod_textaddr;

	fill_vram(0x00FF0000);

	mod = (SceModule2*)sceKernelFindModuleByAddress(regs->epc);

	if(mod != NULL) {
		modname = mod->modname;
		mod_textaddr = mod->text_addr;
	} else {
		modname = "Unknown";
		mod_textaddr = 0x00000000;
	}

	printk("%-21s: %08X\n", "PSP firmware version", sceKernelDevkitVersion());
	printk("%-21s: 0%dg\n", "PSP type", sceKernelGetModel()+1);
	sceRtcGetCurrentClockLocalTime(&tm);
	sprintf(timestr, "%u-%u-%u %02u:%02u:%02u", tm.year, tm.month, tm.day, tm.hour, tm.minutes, tm.seconds);
	printk("%-21s: %s\n", "Crash time", timestr);
	printk("%-21s: 0x%03X\n", "Api type", sceKernelInitApitype());

	printk("\n");

	printk("Exception details:\n\n");
	printk("Exception - %s\n", codeTxt[(regs->cause >> 2) & 31]);
	printk("EPC       - %08X / %s+0x%08X\n", (int) regs->epc, modname, (u32) (regs->epc - mod_textaddr));
	printk("Cause     - %08X\n", (int) regs->cause);
	printk("Status    - %08X\n", (int) regs->status);
	printk("BadVAddr  - %08X\n", (int) regs->badvaddr);

	for (i = 0; i < 32; i += 4) {
		printk("%s:%08X %s:%08X %s:%08X %s:%08X\n",
				regName[i], (int) regs->r[i], regName[i + 1],
				regs->r[i + 1], regName[i + 2],
				regs->r[i + 2], regName[i + 3],
				regs->r[i + 3]);
	}

	printk("\n");

	found = pspDebugGetStackTrace2(regs, traces, MAX_BACKTRACE_NUM);
	
	printk("Call Trace:\n");

	for (i = 0; i < found; ++i) {
		printk("\t%d: caller %08X(%08X) func %08X(%08X)\n", i,
				(u32) traces[i].call_addr,
				(u32) traces[i].call_addr - mod_textaddr,
				(u32) traces[i].func_addr,
				(u32) traces[i].func_addr - mod_textaddr);
	}

	printk_sync();

	sceKernelDelayThread(500000);
	reboot_vsh_with_error(0xC01DB12D);
	sceKernelDelayThread(500000);
	sctrlKernelExitVSH(NULL);

	while(1) {
		sceKernelDelayThread(100000);;
	}
}

static int crash_thread(SceSize args, void *argp)
{
	sceKernelDelayThread(30 * 1000000L);
	printk("%s: crashing...\n", __func__);

	while (1) {
		_sw(0, 0);
	}

	return 0;
}

int module_start(SceSize args, void *argp)
{
	int ret = 0;

	if(psplinkPresent() == 1) {
		return 0;
	}

	printk_init("ms0:/exception.txt");
	curr_handler = ExceptionHandler;
	ret = sceKernelRegisterDefaultExceptionHandler(&myDebugExceptionHandler);

	if( 0 ) {
		int thid;

		thid = sceKernelCreateThread("crash_thread", &crash_thread, 0x10, 0x1000, 0, 0);

		sceKernelStartThread(thid, 0, NULL);
	}

	return 0;
}

int module_stop(SceSize args, void *argp)
{
	return 0;
}
