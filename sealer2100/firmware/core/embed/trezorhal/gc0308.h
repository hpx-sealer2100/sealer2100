#ifndef __GC0308_H__
#define __GC0308_H__
#include <stdbool.h>
#define GC0308_ID 0x9b
int gc0308_init(void);
int gc0308_read_id(int *id);
int gc0308_vertical_mirror(bool mirror);
int gc0308_set_window(int width, int height);

#endif /* __GC0308_H__ */
