#ifndef LIBDERP_COMMON_H
#define LIBDERP_COMMON_H

#include <stddef.h>

struct map_pair
{
	void *k;
	void *v;
};

typedef int comparator(const void *, const void *, void *aux);
typedef unsigned long hashfn(const void *);
typedef void dtor(void *, void *aux);

/* stdlib func wrappers that take (and ignore) aux param */

dtor       derp_free;
comparator derp_strcmp;

/* if you want something other than malloc/realloc/free */

void derp_use_alloc_funcs(
	void *(*m)(size_t n),
	void *(*r)(void *p, size_t n),
	void  (*f)(void *p)
);

#endif
