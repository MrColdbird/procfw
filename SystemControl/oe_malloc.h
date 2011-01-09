#pragma once

int oe_mallocinit();
void *oe_malloc(size_t size);
void oe_free(void *p);
int oe_mallocterminate();
