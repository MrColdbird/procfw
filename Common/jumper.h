#ifndef _JUMPER_H_
#define _JUMPER_H_

#define GET_CALL_ADDR(i) ((i & 0x03ffffff) << 2)

//maximum entry count (2048 for user, 2048 for kernel)
#define JUMPER_MAX_COUNT 2048

//init user memory (only required if you intend to trace user modules)
int initUserTraceMemory(void);

//trace log function
void dumpJAL(u32 target, u32 ra, u32 result);

//single trace install
void installJALTrace(u32 address);

//module trace install
void installModuleJALTrace(SceModule2 * module);

//clear tracing table
void clearTraceTable(void);

#endif
