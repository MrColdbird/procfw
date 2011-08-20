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
