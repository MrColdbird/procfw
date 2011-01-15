#ifndef SYSTEMCTRL_PRIVATE_H
#define SYSTEMCTRL_PRIVATE_H

// Here goes the exported but unpublic functions

int oe_mallocinit();
void *oe_malloc(size_t size);
void oe_free(void *p);
int oe_mallocterminate();

char *GetUmdFile(void);
void SetUmdFile(char *file);

#endif
