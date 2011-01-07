#ifndef MAIN_H
#define MAIN_H

extern u32 psp_model;
extern u32 psp_fw_version;

void syspatch_init();
void setup_module_handler(void);

#endif
