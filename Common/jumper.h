#ifndef _JUMPER_H_
#define _JUMPER_H_

#define GET_CALL_ADDR(i) ((i & 0x03ffffff) << 2)

//maximum entry count
#define JUMPER_MAX_COUNT 2048

//single trace install
int installJALTrace(u32 address);

//module trace install
void installModuleJALTrace(SceModule2 * module);

//clear tracing table
void clearTraceTable(void);

#endif
