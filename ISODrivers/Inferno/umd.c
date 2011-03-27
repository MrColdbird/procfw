#include <pspkernel.h>
#include <pspreg.h>
#include <stdio.h>
#include <string.h>
#include <systemctrl.h>
#include <systemctrl_se.h>
#include <pspsysmem_kernel.h>
#include <pspumd.h>
#include <psprtc.h>
#include "utils.h"
#include "printk.h"
#include "libs.h"
#include "utils.h"
#include "systemctrl.h"
#include "systemctrl_se.h"
#include "systemctrl_private.h"
#include "inferno.h"

extern int sceKernelGetCompiledSdkVersion(void);

// 0x000027A8
SceUID g_mediaman_semaid = -1;

// 0x000027AC
int g_umd_error_status = 0;

// 0x000027B0
int g_drive_status = 0;

// 0x00002794
u32 g_prev_gp = 0;
int (*g_00002798)(int, int, int) = 0;
u32 g_0000279C = 0;
u32 g_000027A0 = 0;

// 0x000027A4
SceUID g_umd_cbid = 0;

extern int sceKernelCancelSema(SceUID semaid, int newcount, int *num_wait_threads);

int sceUmdCheckMedium(void)
{
	int ret;

	ret = 1;
	printk("%s: -> 0x%08X\n", __func__, ret);

	return ret;
}

int sceUmdReplacePermit(void)
{
	int ret;

	ret = 0;
	printk("%s: -> 0x%08X\n", __func__, ret);

	return ret;
}

int sceUmdReplaceProhibit(void)
{
	int ret;

	ret = 0;
	printk("%s: -> 0x%08X\n", __func__, ret);

	return ret;
}

// 0x00001A14
static void do_umd_notify(int arg)
{
	if(g_umd_cbid < 0) {
		return;
	}

	sceKernelNotifyCallback(g_umd_cbid, arg);
}

int sceUmdRegisterUMDCallBack(int cbid)
{
	int ret;
	u32 k1;

	k1 = pspSdkSetK1(0);
	ret = sceKernelGetThreadmanIdType(cbid);

	if(ret != SCE_KERNEL_TMID_Callback) {
		ret = 0x80010016;
		goto exit;
	}

	g_umd_cbid = cbid;
	do_umd_notify(g_drive_status);
	ret = 0;

exit:
	pspSdkSetK1(k1);

	return ret;
}

int sceUmdUnRegisterUMDCallBack(int cbid)
{
	u32 k1;
	int ret;
	uidControlBlock *type;

	k1 = pspSdkSetK1(0);
	ret = sceKernelGetUIDcontrolBlock(cbid, &type);

	if(ret == 0) {
		if(g_umd_cbid == cbid) {
			g_umd_cbid = -1;
		}
	}

	pspSdkSetK1(k1);

	return 0;
}

int sceUmdGetDiscInfo(pspUmdInfo *info)
{
	int ret;
	u32 k1;

	k1 = pspSdkSetK1(0);

	if(info != NULL && sizeof(*info) == info->size) {
		info->type = PSP_UMD_TYPE_GAME;
		ret = 0;
	} else {
		ret = 0x80010016;
	}

	pspSdkSetK1(k1);
	printk("%s: -> 0x%08X\n", __func__, ret);

	return ret;
}

int sceUmdCancelWaitDriveStat(void)
{
	u32 k1;
	int ret;

	k1 = pspSdkSetK1(0);
	ret = sceKernelCancelSema(g_mediaman_semaid, -1, NULL);
	pspSdkSetK1(k1);

	return ret;
}

u32 sceUmdGetErrorStatus(void)
{
	printk("%s: -> 0x%08X\n", __func__, g_umd_error_status);

	return g_umd_error_status;
}

void sceUmdSetErrorStatus(u32 status)
{
	g_umd_error_status = status;
	printk("%s: -> 0x%08X\n", __func__, g_umd_error_status);
}

int sceUmdGetDriveStat(void)
{
//	printk("%s: -> 0x%08X\n", __func__, g_drive_status);
	
	return g_drive_status;
}

u32 sceUmdGetDriveStatus(u32 status)
{
	printk("%s: -> 0x%08X\n", __func__, g_drive_status);
	
	return g_drive_status;
}

int sceUmdMan_driver_D37B6422(void)
{
	int ret;

	ret = 0;
	printk("%s: -> 0x%08X\n", __func__, ret);

	return ret;
}

int sceUmdMan_driver_6A1FB0DD(void)
{
	int ret;

	ret = 0;
	printk("%s: -> 0x%08X\n", __func__, ret);

	return ret;
}

int sceUmdMan_driver_7DF4C4DA(u32 a0)
{
	if(g_0000279C != a0) {
		return 0x80010002;
	}

	g_prev_gp = 0;
	g_00002798 = 0;
	g_0000279C = 0;
	g_000027A0 = 0;

	return 0;
}

int sceUmdMan_driver_F7A0D0D9(void)
{
	int ret;

	ret = 0;
	printk("%s: -> 0x%08X\n", __func__, ret);

	return ret;
}

static inline u32 get_gp(void)
{
	u32 gp;

	__asm__ volatile ("move %0, $gp;" : "=r"(gp));

	return gp;
}

static inline void set_gp(u32 gp)
{
	__asm__ volatile ("move $gp, %0;" : :"r"(gp));
}

// for now 6.20/6.35 share the same patch
int sceUmdMan_driver_4FFAB8DA(u32 a0, u32 a1, u32 a2)
{
	SceModule2 *mod;
	u32 text_addr, intr;
	int i;

	if(0 != g_0000279C) {
		return 0x8001000C;
	}

	g_0000279C = a0;
	g_prev_gp = get_gp();
	g_000027A0 = a2;
	g_00002798 = (void*)a1;
	
	mod = (SceModule2*)sceKernelFindModuleByName("sceIsofs_driver");
	text_addr = mod->text_addr;

	intr = 0x00001021; // move $v0, $zr

	for(i=0; i<NELEMS(g_offs->patches); ++i) {
		_sw(intr, g_offs->patches[i] + text_addr);
	}

	sync_cache();

	if(0 == g_00002798) {
		return 0;
	}

	set_gp(g_prev_gp);
	(*g_00002798)(g_0000279C, g_000027A0, 1);

	return 0;
}

// 0x000014F0
u32 sceUmdClearDriveStatus(u32 mask)
{
	g_drive_status &= mask;

	return g_drive_status;
}

int sceUmd9660_driver_63342C0F(void)
{
	int ret;

	ret = 0;
	printk("%s: -> 0x%08X\n", __func__, ret);

	return ret;
}

int sceUmd9660_driver_6FFFEE54(void)
{
	int ret;

	ret = 0;
	printk("%s: -> 0x%08X\n", __func__, ret);

	return ret;
}

int sceUmd9660_driver_7CB291E3(void)
{
	int ret;

	ret = 0;
	printk("%s: -> 0x%08X\n", __func__, ret);

	return ret;
}

// 0x00001758
int wait_umd_stat(int stat, SceUInt timeout, int use_callback)
{
	u32 k1;
	int (*func)(SceUID, int, SceUInt*), ret;

	k1 = pspSdkSetK1(0);

	if(!(stat & (PSP_UMD_NOT_PRESENT | PSP_UMD_PRESENT | PSP_UMD_INITING | PSP_UMD_INITED | PSP_UMD_READY))) {
		ret = 0x80010016;
		goto exit;
	}

	if(stat & PSP_UMD_READY) {
		g_drive_status |= PSP_UMD_READY;
		ret = 0;
		goto exit;
	}

	if(stat & (PSP_UMD_PRESENT | PSP_UMD_INITED)) {
		ret = 0;
		goto exit;
	}

	if(stat & (PSP_UMD_NOT_PRESENT | PSP_UMD_INITING)) {
		ret = 0;
		goto exit;
	}

	if(use_callback) {
		func = &sceKernelWaitSemaCB;
	} else {
		func = &sceKernelWaitSema;
	}

	if(timeout == 0) {
		(*func)(g_mediaman_semaid, 1, NULL);
	} else {
		(*func)(g_mediaman_semaid, 1, &timeout);
	}

	ret = 0;

exit:
	if(use_callback) {
		if(g_game_fix_type == 2) {
			sceKernelCheckCallback();
		}
	}

	pspSdkSetK1(k1);

	return ret;
}

int sceUmdWaitDriveStatWithTimer(int stat, SceUInt timeout)
{
	return wait_umd_stat(stat, timeout, 0);
}

int sceUmdWaitDriveStatCB(int stat, SceUInt timeout)
{
	return wait_umd_stat(stat, timeout, 1);
}

int sceUmdWaitDriveStat(int stat)
{
	return wait_umd_stat(stat, 0, 0);
}

int sceUmdActivate(int unit, const char* drive)
{
	u32 k1;
	int value;

	k1 = pspSdkSetK1(0);

	if(0 != strcmp(drive, "disc0:")) {
		pspSdkSetK1(k1);

		return 0x80010016;
	}

	value = 1;
	sceIoAssign(drive, "umd0:", "isofs0:", 1, &value, sizeof(value));
	g_drive_status = PSP_UMD_PRESENT | PSP_UMD_INITED | PSP_UMD_READY;

	if(g_game_fix_type == 1) {
		do_umd_notify(PSP_UMD_PRESENT | PSP_UMD_READY);
		pspSdkSetK1(k1);

		return 0;
	}

	if(g_drive_status & PSP_UMD_READY) {
		pspSdkSetK1(k1);

		return 0;
	}

	do_umd_notify(PSP_UMD_PRESENT | PSP_UMD_INITED | PSP_UMD_READY);
	pspSdkSetK1(k1);

	return 0;
}

int sceUmdDeactivate(int unit, const char *drive)
{
	int ret;
	u32 k1;

	k1 = pspSdkSetK1(0);
	ret = sceIoUnassign(drive);

	if(ret < 0) {
		pspSdkSetK1(k1);

		return ret;
	}

	g_drive_status = PSP_UMD_PRESENT | PSP_UMD_INITED;
	do_umd_notify(PSP_UMD_PRESENT | PSP_UMD_INITED);
	pspSdkSetK1(k1);

	return ret;
}

int sceUmdGetErrorStat(void)
{
	u32 k1;
	int ret;

	k1 = pspSdkSetK1(0);
	ret = g_umd_error_status;
	pspSdkSetK1(k1);

	return ret;
}

// 0x000018A4
void sceUmdSetDriveStatus(u32 status)
{
	u32 intr;

	intr = sceKernelCpuSuspendIntr();

	if(!(status & PSP_UMD_NOT_PRESENT)) {
		if(status & (PSP_UMD_PRESENT | PSP_UMD_CHANGED | PSP_UMD_INITING | PSP_UMD_INITED | PSP_UMD_READY)) {
			g_drive_status &= ~PSP_UMD_NOT_PRESENT;
		}
	} else {
		g_drive_status &= ~(PSP_UMD_PRESENT | PSP_UMD_CHANGED | PSP_UMD_INITING | PSP_UMD_INITED | PSP_UMD_READY);
	}

	if(!(status & PSP_UMD_INITING)) {
		if(status & (PSP_UMD_INITED | PSP_UMD_READY)) {
			g_drive_status &= ~(PSP_UMD_INITING);
		}
	} else {
		g_drive_status &= ~(PSP_UMD_INITED | PSP_UMD_READY);
	}

	g_drive_status |= status;

	if(g_drive_status & PSP_UMD_READY) {
		g_drive_status |= PSP_UMD_INITED;
	}

	if(g_drive_status & PSP_UMD_INITED) {
		g_drive_status |= PSP_UMD_PRESENT;
		sceUmdSetErrorStatus(0);
	}

	sceKernelCpuResumeIntr(intr);
}

int sceUmd_004F4BE5(int orig_error_code)
{
	u32 compiled_version;
	int error_code = orig_error_code;

	if(error_code == 0) {
		goto exit;
	}

	compiled_version = sceKernelGetCompiledSdkVersion();

	if(compiled_version == 0) {
		if(error_code == 0x80010074) {
			error_code = 0x8001006E;
		} else if(error_code == 0x80010070) {
			error_code = 0x80010062;
		} else if(error_code == 0x8001005B) {
			error_code = 0x80010062;
		} else if(error_code == 0x80010071) {
			error_code = 0x80010067;
		} else if(error_code == 0x80010086) {
			error_code = 0x8001B000;
		} else if(error_code == 0x80010087) {
			error_code = 0x8001007B;
		} else if(error_code == 0x8001B006) {
			error_code = 0x8001007C;
		}
	}

exit:
//	printk("%s: 0x%08X -> 0x%08X\n", __func__, orig_error_code, error_code);

	return error_code;
}
