#include "vector.h"

#include <assert.h>
#include <errno.h>
#include <stdint.h>
#include <stdlib.h>

#define INITIAL_CAPACITY 64

#ifdef NDEBUG
	#define CHECK(x) (x)
#else
	#define CHECK(x) _check(x)
#endif

static vector *
_check(vector *v)
{
	assert(v);
	assert(v->capacity > 0);
	assert(v->length <= v->capacity);
	assert(v->length < SIZE_MAX);
	return v;
}

vector *
v_new(void (*elt_dtor)(void *))
{
	vector *v   = malloc(sizeof *v);
	void **elts = malloc(INITIAL_CAPACITY * sizeof *elts);
	if (!v || !elts)
	{
		free(v);
		free(elts);
		return NULL;
	}
	*v = (vector){
		.capacity = INITIAL_CAPACITY,
		.elts = elts,
		.elt_dtor = elt_dtor
	};
	return CHECK(v);
}

void
v_free(vector *v)
{
	if (!v)
		return;
	v_clear(v);
	free(v->elts);
	free(v);
}

size_t
v_length(const vector *v)
{
	return v ? v->length : SIZE_MAX;
}

bool
v_set_length(vector *v, size_t len)
{
	if (!v || len == SIZE_MAX)
		return false;
	if (v->elt_dtor) /* free any, if necessary */
		for (size_t i = len; i < v->length; i++)
			v->elt_dtor(v->elts[i]);
	if (!v_reserve_capacity(v, len))
		return false;
	for (size_t i = v->length; i < len; i++)
		v->elts[i] = NULL;
	v->length = len;

	(void)CHECK(v);
	return true;
}

size_t
v_capacity(const vector *v)
{
	return v ? v->capacity : 0;
}

size_t
v_reserve_capacity(vector *v, size_t desired)
{
	if (!v)
		return 0;
	if (desired <= v->capacity)
		return v->capacity;
	size_t n = v->capacity;
	while (n < desired && n < SIZE_MAX/2)
		n *= 2;
	if (n < desired)
		n = desired; /* desired >= SIZE_MAX/2 */
	void **enlarged = realloc(v->elts, n);
	if (!enlarged)
		return v->capacity;
	v->elts = enlarged;
	v->capacity = n;

	(void)CHECK(v);
	return n;
}

bool
v_is_empty(const vector *v)
{
	return v_length(v) == 0;
}

void *
v_at(const vector *v, size_t i)
{
	if (!v || i >= v->length)
	{
		errno = EDOM;
		return NULL;
	}
	return v->elts[i];
}

void *
v_first(const vector *v)
{
	return v_at(v, 0);
}

void *
v_last(const vector *v)
{
	/* when length is 0, length-1 is SIZE_MAX */
	return v_at(v, v_length(v)-1);
}

bool
v_append(vector *v, void *e)
{
	return v_insert(v, v_length(v), e);
}

bool
v_prepend(vector *v, void *e)
{
	return v_insert(v, 0, e);
}

void *
v_remove_first(vector *v)
{
	return v_remove(v, 0, 1);
}

void *
v_remove_last(vector *v)
{
	return v_remove(v, v_length(v)-1, 1);
}

bool
v_swap(vector *v, size_t i, size_t j)
{
	if (!v || i >= v->length || j >= v->length)
		return false;
	void *t = v->elts[i];
	v->elts[i] = v->elts[j];
	v->elts[j] = t;

	(void)CHECK(v);
	return true;
}

void
v_clear(vector *v)
{
	v_set_length(v, 0);
}

size_t
v_find_index(const vector *v, const void *needle,
             int (*cmp)(const void *, const void *))
{
	if (!v)
		return SIZE_MAX;
	for (size_t i = 0; i < v->length; i++)
		if (cmp(v->elts[i], needle) == 0)
			return i;
	return SIZE_MAX;
}

size_t
v_find_index_last(const vector *v, const void *needle,
                  int (*cmp)(const void *, const void *))
{
	if (!v)
		return SIZE_MAX;
	for (size_t i = v->length-1; i < SIZE_MAX; i--)
		if (cmp(v->elts[i], needle) == 0)
			return i;
	return SIZE_MAX;
}
