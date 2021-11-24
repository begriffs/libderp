#ifndef LIBDERP_COMMON_H
#define LIBDERP_COMMON_H

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

#endif
