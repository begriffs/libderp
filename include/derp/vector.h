#ifndef LIBDERP_VECTOR_H
#define LIBDERP_VECTOR_H

#include "derp/common.h"

#include <stdbool.h>
#include <stddef.h>

typedef struct vector vector;

vector * v_new(void);
void     v_free(vector *);
void     v_dtor(vector *, dtor *, void *);
size_t   v_length(const vector *);
bool     v_set_length(vector *, size_t);
size_t   v_capacity(const vector *);
size_t   v_reserve_capacity(vector *, size_t);
bool     v_is_empty(const vector *);
void *   v_at(const vector *, size_t);
void *   v_first(const vector *);
void *   v_last(const vector *);
bool     v_append(vector *, void *);
bool     v_prepend(vector *, void *);
void *   v_remove_first(vector *);
void *   v_remove_last(vector *);
bool     v_insert(vector *, size_t, void *);
void *   v_remove(vector *, size_t);
bool     v_swap(vector *, size_t, size_t);
void     v_clear(vector *);
size_t   v_find_index(const vector *, const void *,
                      comparator *, void *aux);
size_t   v_find_last_index(const vector *, const void *,
                           comparator *, void *aux);
bool     v_sort(vector *, comparator *, void *aux);
bool     v_reverse(vector *);

#endif
