#ifndef DERP_ALLOC_H
#define DERP_ALLOC_H

#include <stddef.h>

extern void *(*internal_malloc)(size_t n);
extern void *(*internal_realloc)(void *p, size_t n);
extern void  (*internal_free)(void *p);

#endif
