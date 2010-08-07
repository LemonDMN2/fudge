#ifndef LIB_MEMORY_H
#define LIB_MEMORY_H

extern int memory_compare(void *dest, void *src, unsigned int count);
extern void *memory_copy(void *dest, void *src, unsigned int count);
extern void *memory_replace(void *dest, char value1, char value2, unsigned int count);
extern void *memory_set(void *dest, char value, unsigned int count);
extern unsigned short *memory_setw(void *dest, short value, unsigned int count);

#endif

