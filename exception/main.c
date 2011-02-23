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

#define MAX_BACKTRACE_NUM 10

PSP_MODULE_INFO("exception", 0x1007, 1, 1);
PSP_MAIN_THREAD_ATTR(0);

PspDebugErrorHandler curr_handler;
PspDebugRegBlock exception_regs;

extern void *myDebugExceptionHandler;
extern int psplinkPresent(void);

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

void ExceptionHandler(PspDebugRegBlock * regs)
{
#if 1
	int i, found;
	PspDebugStackTrace traces[MAX_BACKTRACE_NUM];
	char testo[512];
	char timestr[80];
	pspTime tm;
	SceModule2 *mod;
	const char *modname;
	u32 mod_textaddr;

	mod = (SceModule2*)sceKernelFindModuleByAddress(regs->epc);

	if(mod != NULL) {
		modname = mod->modname;
		mod_textaddr = mod->text_addr;
	} else {
		modname = "Unknown";
		mod_textaddr = 0x00000000;
	}

	printk("%-21s: %08X\n", "PSP firmware version",
			sceKernelDevkitVersion());
	printk("%-21s: 0%dg\n", "PSP type", sceKernelGetModel());
	sceRtcGetCurrentClockLocalTime(&tm);
	sprintf(timestr, "%u-%u-%u %02u:%02u:%02u", tm.year, tm.month,
			tm.day, tm.hour, tm.minutes, tm.seconds);

	printk("%-21s: %s\n", "Crash time", timestr);

	printk("\n");

	printk("Exception details:\n\n");
	printk("Exception - %s\n",
			codeTxt[(regs->cause >> 2) & 31]);
	printk("EPC       - %08X / %s.text + %08X\n",
			(int) regs->epc, modname,
			(unsigned int) (regs->epc - mod_textaddr));
	printk("Cause     - %08X\n", (int) regs->cause);
	printk("Status    - %08X\n", (int) regs->status);
	printk("BadVAddr  - %08X\n", (int) regs->badvaddr);

	for (i = 0; i < 32; i += 4) {
		printk("%s:%08X %s:%08X %s:%08X %s:%08X\n",
				regName[i], (int) regs->r[i], regName[i + 1],
				(int) regs->r[i + 1], regName[i + 2],
				(int) regs->r[i + 2], regName[i + 3],
				(int) regs->r[i + 3]);
	}

	printk("\n");

	found = pspDebugGetStackTrace2(regs, traces, MAX_BACKTRACE_NUM);
	
	printk("Call Trace:\n");

	for (i = 0; i < found; ++i) {
		sprintf(testo,
				"\t%d: caller %08X(%08X) func %08X(%08X)\n", i,
				(unsigned int) traces[i].call_addr,
				(unsigned int) traces[i].call_addr - mod_textaddr,
				(unsigned int) traces[i].func_addr,
				(unsigned int) traces[i].func_addr - mod_textaddr);
	}

	printk_sync();

	sceKernelDelayThread(100000);
#endif
	sctrlKernelExitVSH(NULL);
}

int module_start(SceSize args, void *argp)
{
	int ret = 0;

	if(psplinkPresent() == 1) {
		return 0;
	}

	printk_init("ms0:/exception.txt");
	printk("exception started\n");
	curr_handler = ExceptionHandler;
	ret = sceKernelRegisterDefaultExceptionHandler(myDebugExceptionHandler);
	printk("%s: sceKernelRegisterDefaultExceptionHandler -> 0x%08X\n", __func__, ret);

	return 0;
}

int module_stop(SceSize args, void *argp)
{
	return 0;
}
