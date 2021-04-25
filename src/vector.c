#include "vector.h"

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define INITIAL_CAPACITY 64

#ifdef NDEBUG
	#define CHECK(x) (void)(x)
#else
	#define CHECK(x) _check(x)
#endif

#define SWAP(x, y) do { void *swaptmp = (x); (x) = (y); (y) = swaptmp; } while (0)

struct vector
{
	size_t length;
	size_t capacity;
	void **elts;
	dtor *elt_dtor;
	void *dtor_aux;
};

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
}

vector *
v_new(void)
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
		.elts = elts
	};
	CHECK(v);
	return v;
}

void
v_dtor(vector *v, dtor *elt_dtor, void *dtor_aux)
{
	if (!v)
		return;
	v->elt_dtor = elt_dtor;
	v->dtor_aux = dtor_aux;
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
	return v ? v->length : 0;
}

bool
v_set_length(vector *v, size_t desired)
{
	if (!v)
		return false;
	if (v->elt_dtor) /* free any, if necessary */
		for (size_t i = desired; i < v->length; i++)
			v->elt_dtor(v->elts[i], v->dtor_aux);
	if (v_reserve_capacity(v, desired) < desired)
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
	return v_length(v) == 0;
}

void *
v_at(const vector *v, size_t i)
{
	if (!v || i >= v->length)
		return NULL;
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
	/* successfully fails when length is 0 */
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
		return NULL;
	void *elt = v->elts[i];
	memmove(v->elts+i, v->elts+i+1, (v->length - (i+1)) * sizeof *v->elts);
	v->length--;

	CHECK(v);
	return elt;
}

bool
v_insert(vector *v, size_t i, void *elt)
{
	if (!v || v_reserve_capacity(v, v->length+1) < v->length+1)
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
		return false;
	SWAP(v->elts[i], v->elts[j]);

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
             comparator *cmp, void *aux)
{
	if (!v || !cmp)
		return SIZE_MAX;
	for (size_t i = 0; i < v->length; i++)
		if (cmp(v->elts[i], needle, aux) == 0)
			return i;
	return SIZE_MAX;
}

size_t
v_find_last_index(const vector *v, const void *needle,
                  comparator *cmp, void *aux)
{
	if (!v || !cmp)
		return SIZE_MAX;
	for (size_t i = v->length-1; i < SIZE_MAX; i--)
		if (cmp(v->elts[i], needle, aux) == 0)
			return i;
	return SIZE_MAX;
}

/* from Bentley, https://www.youtube.com/watch?v=QvgYAQzg1z8 */
static void
_quicksort(vector *v, size_t lo, size_t hi,
           comparator *cmp, void *aux)
{
	size_t i, m = lo;
	if (lo >= hi)
		return;
	for (i = lo+1; i <= hi; i++)
		if (cmp(v->elts[i], v->elts[lo], aux) < 0)
		{
			m++;
			SWAP(v->elts[i], v->elts[m]);
		}
	SWAP(v->elts[lo], v->elts[m]);

	if (m > 0) /* avoid wrapping size_t */
		_quicksort(v, lo, m-1, cmp, aux);
	_quicksort(v, m+1, hi, cmp, aux);
}

bool
v_sort(vector *v, comparator *cmp, void *aux)
{
	if (!v || !cmp)
		return false;
	_quicksort(v, 0, v->length-1, cmp, aux);

	CHECK(v);
	return true;
}

bool
v_reverse(vector *v)
{
	size_t n = v_length(v);
	if (n < 2)
		return true;
	for (size_t i = n-1; i >= n/2; i--)
		SWAP(v->elts[i], v->elts[n-i-1]);
	CHECK(v);
	return true;
}
