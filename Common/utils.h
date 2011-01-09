#ifndef UTILS_H
#define UTILS_H

#define MAKE_JUMP(f) (0x08000000 | (((u32)(f) >> 2) & 0x03ffffff))
#define MAKE_CALL(f) (0x0C000000 | (((u32)(f) >> 2) & 0x03ffffff)) 

#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define MIN(a, b) (((a) < (b)) ? (a) : (b))

#define NELEMS(a) (sizeof(a) / sizeof(a[0]))

int sceKernelGetModel(void);

void sync_cache(void);

/**
 * @return interrupted status
 * 0 - we are definitely in interrupt disabled status. And the interrupt status won't change as long as our code didn't
 * 1 - we are in interrupt enable status. but the interrupt status would change in later code
 */
int is_cpu_intr_enable(void);

void fill_vram(u32 color);

#ifdef DEBUG
void hexdump(void *addr, int size);
#else
static inline void hexdump(void *addr, int size)
{
}
#endif

#endif
