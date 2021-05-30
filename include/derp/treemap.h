#ifndef LIBDERP_TREEMAP_H
#define LIBDERP_TREEMAP_H

#include "derp/common.h"

#include <stdbool.h>
#include <stddef.h>

typedef struct treemap treemap;
typedef struct tm_iter tm_iter;

treemap * tm_new(comparator *, void *cmp_aux);
void      tm_free(treemap *);
void      tm_dtor(treemap *, dtor *key_dtor, dtor *val_dtor, void *aux);
size_t    tm_length(const treemap *);
bool      tm_is_empty(const treemap *);
void *    tm_at(const treemap *, const void *);
bool      tm_insert(treemap *, void *key, void *val);
bool      tm_remove(treemap *, void *);
void      tm_clear(treemap *);

tm_iter*         tm_iter_begin(treemap *);
struct map_pair* tm_iter_next(tm_iter *);
void             tm_iter_free(tm_iter *);

#endif
