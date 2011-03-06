#ifndef MAIN_H
#define MAIN_H

typedef struct {
	void *next;
	u64 mtime;
	u8 folder;
	char name;
} Category;

void ClearCategories();
Category *GetNextCategory(Category *prev);
void AddCategory(char *category, u64 mtime);
void DelCategory(char *category);
int CountCategories();

int scePaf_strlen(const char *path);
int scePaf_memset(void *buff ,int c ,int size);
int scePaf_memcmp(const void *path , const void *name , int c);
int scePaf_sprintf(char *buffer , const char *format , ...);
int scePaf_snprintf(char *buffer,int c , const char *format, ...);
int scePaf_memcpy(void *path , void *name , int size);
int scePaf_strcpy(char *path , const char *name);
int scePaf_strcmp(const char *path , const char *name);
void *scePaf_malloc(int size );
int scePaf_free(void *buffer);

void SystemPatch(u32 text_addr);
void ExportPatch(u32 text_addr);
void ClearCaches(void);

#endif
