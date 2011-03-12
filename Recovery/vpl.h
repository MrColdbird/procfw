#ifndef VPL_H
#define VPL_H

#define VPL_POOL_SIZE (32 * 1024)

void vpl_init(void);
void vpl_finish(void);

void *vpl_alloc(int size);
char *vpl_strdup(const char *str);
void vpl_free(void *p);
void *vpl_realloc(void *ptr, size_t size);

#endif
