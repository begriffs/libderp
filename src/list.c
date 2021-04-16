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

static void        _check(const list *l);
static list_item * _merge(list_item *, list_item *,
                          int (*)(const void*, const void*));
static list_item * _bisect(list_item *);
static list_item * _sort(list_item *,
                         int (*)(const void*, const void*));

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

bool
l_append(list *l, void *data)
{
	return l_insert_after(l, l_last(l), data);
}

bool
l_prepend(list *l, void *data)
{
	return l_insert(l, l_first(l), data);
}

list_item *
l_remove_first(list *l)
{
	list_item *old_head = l_first(l);
	return l_remove(l, old_head) ? old_head : NULL;
}

list_item *
l_remove_last(list *l)
{
	list_item *old_tail = l_last(l);
	return l_remove(l, old_tail) ? old_tail : NULL;
}

bool
l_remove(list *l, list_item *li)
{
	if (!l || !li || l->length < 1)
		return false;
	list_item *p = li->prev, *n = li->next;
	if (p)
		p->next = n;
	if (n)
		n->prev = p;
	if (li == l->head)
		l->head = n;
	if (li == l->tail)
		l->tail = p;
	l->length--;

	CHECK(l);
	return true;
}

bool
l_insert(list *l, list_item *pos, void *data)
{
	if (!l)
		return false;
	if (!pos)
		pos = l->head;
	list_item *li = malloc(sizeof *li);
	if (!li)
		return false;
	*li = (list_item){
		.prev = pos ? pos->prev : NULL,
		.next = pos,
		.data = data
	};
	if (pos)
	{
		if (pos->prev)
			pos->prev->next = li;
		pos->prev = li;
	}

	if (!l->head)
		l->head = l->tail = li;
	else if (pos == l->head)
		l->head = li;
	l->length++;

	CHECK(l);
	return true;
}

bool
l_insert_after(list *l, list_item *pos, void *data)
{
	if (!l)
		return false;
	if (!pos)
		pos = l->tail;
	list_item *li = malloc(sizeof *li);
	if (!li)
		return false;
	*li = (list_item){
		.prev = pos,
		.next = pos ? pos->next : NULL,
		.data = data
	};
	if (pos)
	{
		if (pos->next)
			pos->next->prev = li;
		pos->next = li;
	}

	if (!l->tail)
		l->head = l->tail = li;
	else if (pos == l->tail)
		l->tail = li;
	l->length++;

	CHECK(l);
	return true;
}

bool
l_clear(list *l)
{
	if (!l)
		return false;
	list_item *li = l_first(l);
	while (li)
	{
		list_item *n = li->next;
		if (l->elt_dtor)
			l->elt_dtor(li->data);
		free(li);
		li = n;
	}
	l->head = l->tail = NULL;
	l->length = 0;

	CHECK(l);
	return true;
}

bool
l_sort(list *l, int (*cmp)(const void*, const void*))
{
	if (!l || !cmp)
		return false;
	if (l_length(l) < 2)
		return true;

	l->head = _sort(l->head, cmp);
	list_item *t = l->head;
	while (t && t->next)
		t = t->next;
	l->tail = t;
	CHECK(l);
	return true;
}


/*** Internals ***/

static void
_check(const list *l)
{
	assert(l);
	assert( (!l->head && l->length == 0) ||
	        ( l->head && l->length != 0) );
	assert( (!l->head && !l->tail) ||
	        ( l->head &&  l->tail) );

	list_item *li = l_first(l);
	while (li && li->next)
		li = li->next;
	assert(li == l_last(l));

	li = l_last(l);
	while (li && li->prev)
		li = li->prev;
	assert(li == l_first(l));
}

static list_item *
_merge(list_item *a, list_item *b,
       int (*cmp)(const void*, const void*))
{
	if (!a)
		return b;
	if (!b)
		return a;
	list_item *ret, *n;
	if (cmp(a->data, b->data) <= 0)
	{
		ret = a;
		n = _merge(a->next, b, cmp);
	}
	else
	{
		ret = b;
		n = _merge(a, b->next, cmp);
	}
	ret->next = n;
	n->prev = ret;
	return ret;
}

static list_item *
_bisect(list_item *li)
{
	assert(li);
	list_item *fast = li;
	while (fast && fast->next)
	{
		li = li->next;
		fast = fast->next->next;
	}
	if (li->prev) /* cut in twain */
		li->prev = li->prev->next = NULL;
	return li;
}

static list_item *
_sort(list_item *l,
      int (*cmp)(const void*, const void*))
{
	assert(l);
	assert(cmp);
	if (!l->next)
		return l;
	list_item *r = _bisect(l);
	l = _sort(l, cmp);
	r = _sort(r, cmp);
	return _merge(l, r, cmp);
}
