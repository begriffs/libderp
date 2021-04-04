#include "list.h"

#include <assert.h>
#include <stdlib.h>

#ifdef NDEBUG
	#define CHECK(x) (void)(x)
#else
	#define CHECK(x) _check(x)
#endif

struct list
{
	list_item *head, *tail;
	void (*elt_dtor)(void *);
	size_t length;
};

static void
_check(const list *l)
{
	assert(l);
	assert( (!l->head && l->length == 0) ||
	        ( l->head && l->length != 0) );
	assert( (!l->head && !l->tail) ||
	        ( l->head &&  l->tail) );
}

list *
l_new(void (*elt_dtor)(void *))
{
	list *l = malloc(sizeof *l);
	if (!l)
		return NULL;
	*l = (list){.elt_dtor = elt_dtor};
	CHECK(l);
	return l;
}

void
l_free(list *l)
{
	if (!l)
		return;
	l_clear(l);
	free(l);
}

size_t
l_length(const list *l)
{
	return l ? l->length : 0;
}

bool
l_is_empty(const list *l)
{
	return !l || !l->head;
}

list_item *
l_first(const list *l)
{
	return l ? l->head : NULL;
}

list_item *
l_last(const list *l)
{
	return l ? l->tail : NULL;
}

