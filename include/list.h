#ifndef LIBDERP_LIST_H
#define LIBDERP_LIST_H

#include "common.h"

#include <stdbool.h>
#include <stddef.h>

/* client should look inside */
typedef struct list_item
{
	void *data;
	struct list_item *prev, *next;
} list_item;

/* but this should be opaque */
typedef struct list list;

list *      l_new(void (*elt_dtor)(void *));
void        l_free(list *);
size_t      l_length(const list *);
bool        l_is_empty(const list *);
list_item * l_first(const list *);
list_item * l_last(const list *);
list_item * l_find(const list *, const void *,
                   comparator *, void *aux);
list_item * l_find_last(const list *, const void *,
                        comparator *, void * aux);
bool        l_append(list *, void *);
bool        l_prepend(list *, void *);
list_item * l_remove_first(list *);
list_item * l_remove_last(list *);
bool        l_remove(list *, list_item *);
bool        l_insert(list *, list_item *, void *);
bool        l_insert_after(list *, list_item *, void *);
bool        l_clear(list *);
bool        l_sort(list *, comparator *, void *aux);

#endif
