#ifndef DIRENT_TRACK_H
#define DIRENT_TRACK_H

struct IoDirentEntry {
	char path[128];
	SceUID dfd, iso_dfd;
	struct IoDirentEntry *next;
};

int dirent_add(SceUID dfd, SceUID iso_dfd, const char *path);
void dirent_remove(struct IoDirentEntry *p);
struct IoDirentEntry *dirent_search(SceUID magic);

#endif
