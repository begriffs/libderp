#include "derp/list.h"

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
	dtor *elt_dtor;
	void *dtor_aux;
	size_t length;
};

static void        _check(const list *l);
static list_item * _merge(list_item *, list_item *,
                          comparator *, void *);
static list_item * _bisect(list_item *);
static list_item * _sort(list_item *,
                         comparator *, void *);

list *
l_new(void)
{
	list *l = malloc(sizeof *l);
	if (!l)
		return NULL;
	*l = (list){0};
	CHECK(l);
	return l;
}

void
l_dtor(list *l, dtor *elt_dtor, void *dtor_aux)
{
	if (!l)
		return;
	l->elt_dtor = elt_dtor;
	l->dtor_aux = dtor_aux;
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

list_item *
l_find(const list *l, const void *needle,
       comparator *cmp, void *aux)
{
	if (!l || !cmp)
		return NULL;
	for (list_item *li = l_first(l); li; li = li->next)
		if (cmp(li->data, needle, aux) == 0)
			return li;
	return NULL;
}

list_item *
l_find_last(const list *l, const void *needle,
            comparator *cmp, void *aux)
{
	if (!l || !cmp)
		return NULL;
	for (list_item *li = l_last(l); li; li = li->prev)
		if (cmp(li->data, needle, aux) == 0)
			return li;
	return NULL;
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

void *
l_remove_first(list *l)
{
	list_item *old_head = l_first(l);
	void *data = old_head ? old_head->data : NULL;
	l_remove(l, old_head);
	return data;
}

void *
l_remove_last(list *l)
{
	list_item *old_tail = l_last(l);
	void *data = old_tail ? old_tail->data : NULL;
	l_remove(l, old_tail);
	return data;
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
	free(li);

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
			l->elt_dtor(li->data, l->dtor_aux);
		free(li);
		li = n;
	}
	l->head = l->tail = NULL;
	l->length = 0;

	CHECK(l);
	return true;
}

bool
l_sort(list *l, comparator *cmp, void *aux)
{
	if (!l || !cmp)
		return false;
	if (l_length(l) < 2)
		return true;

	l->head = _sort(l->head, cmp, aux);
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

	/* for extra assurance, could test that the list
	 * is connected front to back and back to front.
	 *
	 * Probably adds a good bit of overhead.
	list_item *li = l_first(l);
	while (li && li->next)
		li = li->next;
	assert(li == l_last(l));

	li = l_last(l);
	while (li && li->prev)
		li = li->prev;
	assert(li == l_first(l));
	*/
}

static list_item *
_merge(list_item *a, list_item *b,
       comparator *cmp, void *aux)
{
	if (!a)
		return b;
	if (!b)
		return a;
	list_item *ret, *n;
	if (cmp(a->data, b->data, aux) <= 0)
	{
		ret = a;
		n = _merge(a->next, b, cmp, aux);
	}
	else
	{
		ret = b;
		n = _merge(a, b->next, cmp, aux);
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
_sort(list_item *l, comparator *cmp, void *aux)
{
	assert(l);
	assert(cmp);
	if (!l->next)
		return l;
	list_item *r = _bisect(l);
	l = _sort(l, cmp, aux);
	r = _sort(r, cmp, aux);
	return _merge(l, r, cmp, aux);
}
