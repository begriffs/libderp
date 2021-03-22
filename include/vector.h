#ifndef LIBGRIFFS_VECTOR_H
#define LIBGRIFFS_VECTOR_H

#include <stdbool.h>
#include <stddef.h>

typedef struct vector
{
	size_t length;
	size_t capacity;
	void **elts;
	void (*elt_dtor)(void *);
} vector;

vector * v_new(void (*elt_dtor)(void *));
void     v_free(vector *);
size_t   v_length(vector *);
bool     v_set_length(vector *, size_t);
size_t   v_capacity(vector *);
size_t   v_reserve_capacity(vector *, size_t);
bool     v_is_empty(vector *);
void *   v_at(vector *, size_t);
void *   v_first(vector *);
void *   v_last(vector *);
bool     v_append(vector *, void *);
bool     v_prepend(vector *, void *);
void *   v_remove_first(vector *);
void *   v_remove_last(vector *);
bool     v_insert(vector *, size_t, void *);
bool     v_remove(vector *, size_t i, size_t len);
bool     v_swap(vector *, size_t, size_t);
bool     v_clear(vector *);
size_t   v_find_index(vector *, void *, int (*)(const void *, const void *));
size_t   v_find_last_index(vector *, void *, int (*)(const void *, const void *));
bool     v_sort(vector *, int (*)(const void *, const void *));

#endif
