#ifndef MAIN_H
#define MAIN_H

#include "systemctrl_se.h"

extern u32 psp_model;
extern u32 psp_fw_version;

extern SEConfig conf;

int vshpatch_init(void);

#endif
