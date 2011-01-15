#ifndef PRINTK_H
#define PRINTK_H

#ifdef DEBUG
int printk_init(const char* filename);
int printk(char *fmt, ...)__attribute__((format (printf, 1, 2)));
int printk_sync(void);
void printk_lock(void);
void printk_unlock(void);
#else
#define printk_init(...)
#define printk(...)
#define printk_sync()
#define printk_lock()
#define printk_unlock()
#endif

#endif

