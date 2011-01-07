#ifndef PRINTK_H
#define PRINTK_H

#ifdef DEBUG
int printk_init(void);
int printk(char *fmt, ...)__attribute__((format (printf, 1, 2)));
int printk_lock(void);
int printk_unlock(void);
#else
#define printk_init()
#define printk(...)
#define printk_lock()
#define printk_unlock()
#endif

#endif

