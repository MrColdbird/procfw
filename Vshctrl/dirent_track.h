#ifndef DIRENT_TRACK_H
#define DIRENT_TRACK_H

struct IoDirentEntry {
	u32 used;
	SceUID dfd, iso_dfd;
	char path[64];
};

#define MAX_DIRENT_NUMBER 2

struct IoDirentEntry *dirent_get_unused(void);
void dirent_add(struct IoDirentEntry *p, SceUID dfd, SceUID iso_dfd, const char *path);
void dirent_remove(struct IoDirentEntry *p);
struct IoDirentEntry *dirent_search(SceUID orig);

#endif
