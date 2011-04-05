#include <pspsdk.h>
#include <pspsysmem_kernel.h>
#include <pspkernel.h>
#include <psputilsforkernel.h>
#include <pspsysevent.h>
#include <pspiofilemgr.h>
#include <stdio.h>
#include <string.h>
#include "main.h"
#include "utils.h"
#include "systemctrl.h"
#include "printk.h"
#include "libs.h"
#include "registry.h"
#include "systemctrl_patch_offset.h"

typedef struct _MemPart {
	u32 *meminfo;
	int offset;
	int size;
} MemPart;

int g_high_memory_enabled = 0;

static u8 g_p8_size = 4;

static inline int is_homebrews_runlevel(void);

static inline u32 *get_partition(int pid)
{
	u32 * (*get_memory_partition)(int pid) = (void *)(g_offs->high_memory_patch.get_partition);

	return (*get_memory_partition)(pid);
}

//prevent umd-cache in homebrew, so we can drain the cache partition.
void patch_umdcache(u32 text_addr)
{
	int ret;

	ret = sceKernelApplicationType();

	if (ret != PSP_INIT_KEYCONFIG_GAME)
		return;

	ret = sceKernelBootFrom();

	if (ret != 0x40 && ret != 0x50)
		return;
	
	//kill module start
	_sw(0x03E00008, text_addr + g_offs->high_memory_patch.umd_cache_module_start);
	_sw(0x24020001, text_addr + g_offs->high_memory_patch.umd_cache_module_start+4);
}

void unlock_high_memory(u32 forced)
{
	if(!is_homebrews_runlevel() && !forced && !g_high_memory_enabled) {
		return;
	}
	
	//unlock memory
	unsigned int i = 0; for(; i < 0x40; i += 4) {
		_sw(0xFFFFFFFF, 0xBC000040 + i);
	}
}

static int display_meminfo(void)
{
#ifdef DEBUG
	int i;
	int pid = 1;
	int max = 256;

	printk("Memory Partitions:\n");
	printk("N  |    BASE    |   SIZE   | TOTALFREE |  MAXFREE  | ATTR |\n");
	printk("---|------------|----------|-----------|-----------|------|\n");

	for(i = pid; i <= max; i++) {
		SceSize total;
		SceSize free;
		PspSysmemPartitionInfo info;

		memset(&info, 0, sizeof(info));
		info.size = sizeof(info);

		if(sctrlKernelQueryMemoryPartitionInfo(i, &info) == 0) {
			free = sctrlKernelPartitionMaxFreeMemSize(i);
			total = sctrlKernelPartitionTotalFreeMemSize(i);
			printk("%-2d | 0x%08X | %8d | %9d | %9d | %04X |\n", 
					i, info.startaddr, info.memsize, total, free, info.attr);
		}
	}
#endif

	return 0;
}

static void modify_partition(MemPart *part)
{
	u32 *meminfo;
	u32 offset, size;

	meminfo = part->meminfo;
	offset = part->offset;
	size = part->size;

	if(meminfo == NULL) {
		return;
	}

	if (offset != 0) {
		meminfo[1] = (offset << 20) + 0x88800000;
	}

	meminfo[2] = size << 20;
	((u32*)(meminfo[4]))[5] = (size << 21) | 0xFC;
}

static inline int is_homebrews_runlevel(void)
{
	int apitype;

	apitype = sceKernelInitApitype();
	
	if(apitype == 0x152 || apitype == 0x141) {
		return 1;
	}

	return 0;
}

void prepatch_partitions(void)
{
	MemPart p8, p11;

	if(!is_homebrews_runlevel()) {
		printk("%s: no need to patch partition, quit\n", __func__);

		return;
	}

	p8.meminfo = get_partition(8);
	p11.meminfo = get_partition(11);

	g_p8_size = p8.size = 1;
	
	if(p11.meminfo != NULL) {
		p8.offset = 56-4-p8.size;
	} else {
		p8.offset = 56-p8.size;
	}

	modify_partition(&p8);

	p11.size = 4;
	p11.offset = 56-4;
	modify_partition(&p11);
	display_meminfo();
}

void patch_partitions(void) 
{
	MemPart p2, p9;
	int max_user_part_size;

	// shut up gcc warning
	(void)display_meminfo;

	p2.meminfo = get_partition(2);
	p9.meminfo = get_partition(9);

	if(g_p2_size == 24 && g_p9_size == 24) {
		p2.size = MAX_HIGH_MEMSIZE;
		p9.size = 0;
	} else {
		p2.size = g_p2_size;
		p9.size = g_p9_size;
	}

	if(get_partition(11) != NULL) {
		max_user_part_size = 56 - 4 - g_p8_size;
	} else {
		max_user_part_size = 56 - g_p8_size;
	}

	if (p2.size + p9.size > max_user_part_size) {
		// reserved 4MB for P11
		int reserved_len;

		reserved_len = p2.size + p9.size - max_user_part_size;

		if(p9.size > reserved_len) {
			p9.size -= reserved_len;
		} else {
			p2.size -= reserved_len - p9.size; 
			p9.size = 0;
		}
	}

	printk("%s: p2/p9 %d/%d\n", __func__, p2.size, p9.size);

	//reset partition length for next reboot
	sctrlHENSetMemory(24, 24);

	p2.offset = 0;
	modify_partition(&p2);

	p9.offset = p2.size;
	modify_partition(&p9);

	g_high_memory_enabled = 1;
	display_meminfo();
	unlock_high_memory(0);
}

// home menu state
#define GAME_STATE 0
#define HOME_STATE 1
#define DEAD_STATE 2
int home_menu_state = 0;

u32 enterbutton = PSP_CTRL_CIRCLE;
u32 cancelbutton = PSP_CTRL_CROSS;

void (*real_button_callback)(int, int, void*) = NULL;

#define PRESSED_BUTTON(button) ((previous & button) == 0 && (current & button) == button)

void button_hook_handler(u32 current, u32 previous, void * argp)
{
	// game state
	if(home_menu_state == 0) {
		// home button pressed
		if(PRESSED_BUTTON(PSP_CTRL_HOME)) {
			home_menu_state = DEAD_STATE;
		}
	}

	// home state
	else if(home_menu_state == 1) {
		// home or cancel button pressed
		if(PRESSED_BUTTON(PSP_CTRL_HOME) || PRESSED_BUTTON(cancelbutton)) {
			home_menu_state = GAME_STATE;
		}

		// up button pressed
		else if(PRESSED_BUTTON(PSP_CTRL_UP)) {
			home_menu_state = DEAD_STATE;
		}
	}

	// deadlock state
	else if(home_menu_state == 2) {
		// filter enter button
		current &= ~enterbutton;
		previous &= ~enterbutton;

		// home or cancel button pressed
		if(PRESSED_BUTTON(PSP_CTRL_HOME) || PRESSED_BUTTON(cancelbutton)) {
			home_menu_state = GAME_STATE;
		}

		// down button pressed
		else if(PRESSED_BUTTON(PSP_CTRL_DOWN)) {
			home_menu_state = HOME_STATE;
		}
	}

	// forward to real handler
	real_button_callback(current, previous, argp);
}

void set_button_callback(int id, u32 mask, void * handler, void * argp)
{
	// impose button callback
	if(id == 0 && mask == 0x21C10000) {
		// save handler
		real_button_callback = handler;

		// register hook handler
		handler = button_hook_handler;
	}

	// register button callback
	sctrlCtrlRegisterButtonCallback(id, mask, handler, argp);
}

void patch_home_menu(SceModule2 * mod)
{
#ifdef CONFIG_635
	int callbacknid = 0x5D8CE0B2;
#endif
#ifdef CONFIG_620
	int callbacknid = 0xEB6CDD17;
#endif
	hook_import_bynid((SceModule*)mod, "sceCtrl_driver", callbacknid, set_button_callback, 0);
	int xisenter = 0;
	get_registry_value("/CONFIG/SYSTEM/XMB", "button_assign", &xisenter);
	if(xisenter) {
		enterbutton = PSP_CTRL_CROSS;
		cancelbutton = PSP_CTRL_CIRCLE;
	}
}
