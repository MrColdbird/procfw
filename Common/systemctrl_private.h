#ifndef SYSTEMCTRL_PRIVATE_H
#define SYSTEMCTRL_PRIVATE_H

// Here goes the exported but unpublic functions

int oe_mallocinit();
void *oe_malloc(size_t size);
void oe_free(void *p);
int oe_mallocterminate();

char *GetUmdFile(void);
void SetUmdFile(char *file);

/**
 * Set the EBOOT.PBP filename which InitForKernel_2213275D returns
 *
 * @return
 * 0 - OK
 * <0 - fail
 */
int sctrlKernelSetUMDEmuFile(const char *iso);

void SetSpeed(int cpuspd, int busspd);

#endif
