#ifndef UTILS_H
#define UTILS_H

#define MAKE_JUMP(f) (0x08000000 | (((u32)(f) >> 2) & 0x03ffffff))
#define MAKE_CALL(f) (0x0C000000 | (((u32)(f) >> 2) & 0x03ffffff)) 

#define NELEMS(a) (sizeof(a) / sizeof(a[0]))

int sceKernelGetModel(void);

void sync_cache(void);

#ifdef DEBUG
void hexdump(void *addr, int size);
#else
static inline void hexdump(void *addr, int size)
{
}
#endif

#endif
