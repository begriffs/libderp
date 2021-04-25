#ifndef LIBDERP_COMMON_H
#define LIBDERP_COMMON_H

typedef int comparator(const void *, const void *, void *aux);
typedef unsigned long hashfn(const void *);
typedef void dtor(void *, void *aux);

#endif
