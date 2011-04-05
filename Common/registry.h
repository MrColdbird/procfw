#ifndef _REGISTRY_H_
#define _REGISTRY_H_

int get_registry_value(const char *dir, const char *name, unsigned int *val);
int set_registry_value(const char *dir, const char *name, unsigned int val);

#endif
