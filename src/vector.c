#include "vector.h"

#include <assert.h>
#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define INITIAL_CAPACITY 64

#ifdef NDEBUG
	#define CHECK(x) (void)(x)
#else
	#define CHECK(x) _check(x)
#endif

static void
_check(const vector *v)
{
	assert(v);
	assert(v->capacity > 0);
	/* test that capacity is a power of two if not maxed out
	 * https://graphics.stanford.edu/~seander/bithacks.html#DetermineIfPowerOf2
	 */
	if (v->capacity < SIZE_MAX)
		assert((v->capacity & (v->capacity - 1)) == 0);
	assert(v->length <= v->capacity);
	assert(v->length < SIZE_MAX);
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
	CHECK(v);
	return v;
}

void
v_free(vector *v)
{
	if (!v)
	{
		errno = EDOM;
		return;
	}
	v_clear(v);
	free(v->elts);
	free(v);
}

size_t
v_length(const vector *v)
{
	if (!v)
	{
		errno = EDOM;
		return SIZE_MAX;
	}
	return v->length;
}

bool
v_set_length(vector *v, size_t desired)
{
	if (!v || desired == SIZE_MAX)
	{
		errno = EDOM;
		return false;
	}
	if (v->elt_dtor) /* free any, if necessary */
		for (size_t i = desired; i < v->length; i++)
			v->elt_dtor(v->elts[i]);
	if (!v_reserve_capacity(v, desired))
		return false;
	for (size_t i = v->length; i < desired; i++)
		v->elts[i] = NULL;
	v->length = desired;

	CHECK(v);
	return true;
}

size_t
v_capacity(const vector *v)
{
	if (!v)
	{
		errno = EDOM;
		return 0;
	}
	return v->capacity;
}

size_t
v_reserve_capacity(vector *v, size_t desired)
{
	if (!v)
	{
		errno = EDOM;
		return 0;
	}
	if (desired <= v->capacity)
		return v->capacity;
	size_t n = v->capacity;
	/* SIZE_MAX is one less than a power of two, so if
	 * we keep looping too long we'll hit zero */
	while (0 < n && n < desired)
		n *= 2;
	if (n == 0)
		n = SIZE_MAX;
	void **enlarged = realloc(v->elts, n);
	if (!enlarged)
		return v->capacity;
	v->elts = enlarged;
	v->capacity = n;

	CHECK(v);
	return n;
}

bool
v_is_empty(const vector *v)
{
	if (!v)
	{
		errno = EDOM;
		return true;
	}
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
	return v_remove(v, 0);
}

void *
v_remove_last(vector *v)
{
	return v_remove(v, v_length(v)-1);
}

void *
v_remove(vector *v, size_t i)
{
	if (!v || i >= v->length)
	{
		errno = EDOM;
		return NULL;
	}
	void *elt = v->elts[i];
	memmove(v->elts+i, v->elts+i+1, (v->length - (i+1)) * sizeof *v->elts);
	v->length--;

	CHECK(v);
	return elt;
}

bool
v_insert(vector *v, size_t i, void *elt)
{
	if (!v || i == SIZE_MAX)
	{
		errno = EDOM;
		return false;
	}
	if (!v_reserve_capacity(v, v->length+1))
		return false;
	memmove(v->elts+i+1, v->elts+i, (v->length - i) * sizeof *v->elts);
	v->elts[i] = elt;
	v->length++;

	CHECK(v);
	return true;
}

bool
v_swap(vector *v, size_t i, size_t j)
{
	if (!v || i >= v->length || j >= v->length)
	{
		errno = EDOM;
		return false;
	}
	void *t = v->elts[i];
	v->elts[i] = v->elts[j];
	v->elts[j] = t;

	CHECK(v);
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
	if (!v || !cmp)
	{
		errno = EDOM;
		return SIZE_MAX;
	}
	for (size_t i = 0; i < v->length; i++)
		if (cmp(v->elts[i], needle) == 0)
			return i;
	return SIZE_MAX;
}

size_t
v_find_last_index(const vector *v, const void *needle,
                  int (*cmp)(const void *, const void *))
{
	if (!v || !cmp)
	{
		errno = EDOM;
		return SIZE_MAX;
	}
	for (size_t i = v->length-1; i < SIZE_MAX; i--)
		if (cmp(v->elts[i], needle) == 0)
			return i;
	return SIZE_MAX;
}

bool
v_sort(vector *v, int (*cmp)(const void *, const void *))
{
	if (!v || !cmp)
	{
		errno = EDOM;
		return false;
	}
	qsort(v->elts, v->length, sizeof v->elts[0], cmp);

	CHECK(v);
	return true;
}

bool
v_reverse(vector *v)
{
	size_t n = v_length(v);
	if (n == SIZE_MAX)
		return false;
	if (n == 0)
		return true;
	for (size_t i = n-1; i >= n/2; i--)
	{
		void *t = v->elts[i];
		v->elts[i] = v->elts[n-i-1];
		v->elts[n-i-1] = t;
	}
	CHECK(v);
	return true;
}
