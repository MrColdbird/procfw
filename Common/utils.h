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

#ifndef UTILS_H
#define UTILS_H

#define FW_639 0x06030910
#define FW_635 0x06030510
#define FW_620 0x06020010

enum {
	PSP_1000 = 0,
	PSP_2000 = 1,
	PSP_3000 = 2,
	PSP_4000 = 3,
	PSP_GO   = 4,
	PSP_7000 = 6,
	PSP_9000 = 8,
};

#define MAKE_JUMP(f) (0x08000000 | (((u32)(f) >> 2) & 0x03ffffff))
#define MAKE_CALL(f) (0x0C000000 | (((u32)(f) >> 2) & 0x03ffffff)) 
#define NOP 0

#define REDIRECT_FUNCTION(new_func, original) \
	do { \
		_sw(MAKE_JUMP(new_func), (original)); \
		_sw(NOP, (original)+4); \
	} while ( 0 );

#define MAKE_DUMMY_FUNCTION_RETURN_0(a) do {\
	_sw(0x03E00008, a);\
	_sw(0x00001021, a + 4);\
} while (0)

#define MAKE_DUMMY_FUNCTION_RETURN_1(a) do {\
	_sw(0x03E00008, a);\
	_sw(0x24020001, a + 4);\
} while (0)

#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define MIN(a, b) (((a) < (b)) ? (a) : (b))

#define NELEMS(a) (sizeof(a) / sizeof(a[0]))

#define PTR_ALIGN_64(p) ((void*)((((u32)p)+64-1)&(~(64-1))))

int sceKernelGetModel(void);

void sync_cache(void);

/**
 * @return interrupted status
 * 0 - we are definitely in interrupt disabled status. And the interrupt status won't change as long as our code didn't
 * 1 - we are in interrupt enable status. but the interrupt status would change in later code
 */
int is_cpu_intr_enable(void);

#ifdef DEBUG
void hexdump(void *addr, int size);
void fill_vram(u32 color);
#else
static inline void hexdump(void *addr, int size)
{
}
static inline void fill_vram(u32 color)
{
}
#endif

int get_device_name(char *device, int size, const char* path);

SceUID get_thread_id(const char *name);

/** Check if user syscall didn't pass kernel memory, as OFW did */
int check_memory(const void *addr, int size);

#endif
