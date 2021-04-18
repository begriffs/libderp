#ifndef LIBDERP_HASHMAP_H
#define LIBDERP_HASHMAP_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct hashmap hashmap;

hashmap * hm_new(size_t,
                 uint_fast32_t (*hash)(const void *),
                 int (*cmp)(const void *, const void *),
                 void (*key_dtor)(void *),
                 void (*val_dtor)(void *));
void      hm_free(hashmap *);
size_t    hm_length(const hashmap *);
bool      hm_is_empty(const hashmap *);
void *    hm_at(const hashmap *, const void *);
bool      hm_insert(hashmap *, void *key, void *val);
bool      hm_remove(hashmap *, void *);
void      hm_clear(hashmap *);

#endif
