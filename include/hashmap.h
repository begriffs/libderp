#ifndef LIBDERP_HASHMAP_H
#define LIBDERP_HASHMAP_H

#include "common.h"
#include "list.h"

#include <stdbool.h>
#include <stddef.h>

typedef struct hashmap hashmap;
typedef struct hm_iter hm_iter;

hashmap * hm_new(size_t, hashfn *, comparator *, void *cmp_aux);
void      hm_free(hashmap *);
void      hm_dtor(hashmap *, dtor *key_dtor, dtor *val_dtor, void *aux);
size_t    hm_length(const hashmap *);
bool      hm_is_empty(const hashmap *);
void *    hm_at(const hashmap *, const void *);
bool      hm_insert(hashmap *, void *key, void *val);
bool      hm_remove(hashmap *, void *);
void      hm_clear(hashmap *);

hm_iter*         hm_iter_begin(hashmap *h);
struct map_pair* hm_iter_next(hm_iter *);
void             hm_iter_free(hm_iter *);

#endif
