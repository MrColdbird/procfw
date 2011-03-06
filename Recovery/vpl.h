#ifndef VPL_H
#define VPL_H

void vpl_init(void);
void vpl_finish(void);

void *vpl_alloc(int size);
char *vpl_strdup(const char *str);
void vpl_free(void *p);

#endif
