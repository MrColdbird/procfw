#ifndef STARGATE_H
#define STARGATE_H

int myPauth_98B83B5D(u8 *p, u32 size, u32 *newsize, u8 *xor_key);
int myPauth_init(void);

void patch_sceMesgLed(void);

#endif
