/*
 * This file is part of PRO CFW.

 * PRO CFW is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * PRO CFW is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with PRO CFW. If not, see <http://www.gnu.org/licenses/ .
 */

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

void installMemoryJALTrace(u32 start, u32 size);

//clear tracing table
void clearTraceTable(void);

#endif
