#include "derp/vector.h"

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>

#define ARRAY_LEN(a) (sizeof(a)/sizeof(*a))

int cmpint(const void *a, const void *b, void *aux)
{
	(void)aux;
	return *(int*)a - *(int*)b;
}

void myfree(void *a, void *aux)
{
	(void)aux;
	free(a);
}

int main(void)
{
	int ivals[] =  {0,1,2,3,4,5,6,7,8,9},
	    ivals2[] = {10,11,12,13,14,15,16,17,18,19};
	size_t i;
	vector *vint = v_new();
	assert(v_length(vint) == 0);
	assert(v_capacity(vint) > 0);
	assert(v_is_empty(vint));
	assert(!v_at(vint, 0));
	assert(!v_at(vint, 1337));

	size_t too_big = SIZE_MAX;
	/* too large a number for our address space */
	assert(v_reserve_capacity(vint, too_big) < too_big);
	/* still too large for address space */
	too_big = 1 + SIZE_MAX/sizeof(void*);
	/* technically possible, but not on today's machines */
	too_big /= 2;
	assert(v_reserve_capacity(vint, too_big) < too_big);

	v_set_length(vint, 2);
	assert(v_length(vint) == 2);
	assert(v_at(vint, 0) == NULL);
	assert(v_at(vint, 1) == NULL);
	v_clear(vint);

	for (i = 0; i < ARRAY_LEN(ivals); i++)
		v_prepend(vint, ivals+i);
	assert(v_length(vint) == ARRAY_LEN(ivals));
	assert(*(int*)v_first(vint) == 9);
	assert(*(int*)v_remove_first(vint) == 9);
	assert(*(int*)v_remove_first(vint) == 8);
	v_sort(vint, cmpint, NULL);
	assert(*(int*)v_first(vint) == 0);

	v_clear(vint);
	assert(v_length(vint) == 0);
	for (i = 0; i < ARRAY_LEN(ivals); i++)
		v_append(vint, ivals+i);
	assert(v_length(vint) == ARRAY_LEN(ivals));
	assert(*(int*)v_last(vint) == 9);
	assert(*(int*)v_remove_last(vint) == 9);
	assert(*(int*)v_remove_last(vint) == 8);

	assert(v_find_index(vint, ivals+5, cmpint, NULL) == 5);
	v_insert(vint, 5, ivals2+5);
	assert(v_find_index(vint, ivals2+5, cmpint, NULL) == 5);
	v_swap(vint, 5, 6);
	assert(v_find_index(vint, ivals2+5, cmpint, NULL) == 6);
	assert(v_find_last_index(vint, ivals2+5, cmpint, NULL) == 6);
	v_remove(vint, 6);
	assert(v_find_index(vint, ivals2+5, cmpint, NULL) == SIZE_MAX);
	assert(v_find_last_index(vint, ivals2+5, cmpint, NULL) == SIZE_MAX);

	assert(v_find_last_index(vint, v_last(vint), cmpint, NULL)
			== v_length(vint)-1);

	v_clear(vint);
	int revme_even[] = {1,2};
	for (i = 0; i < ARRAY_LEN(revme_even); i++)
		v_append(vint, revme_even+i);
	v_reverse(vint);
	assert(*(int*)v_at(vint, 0) == 2);
	assert(*(int*)v_at(vint, 1) == 1);

	v_clear(vint);
	int revme_odd[] = {1,2,3};
	for (i = 0; i < ARRAY_LEN(revme_odd); i++)
		v_append(vint, revme_odd+i);
	v_reverse(vint);
	assert(*(int*)v_at(vint, 0) == 3);
	assert(*(int*)v_at(vint, 1) == 2);
	assert(*(int*)v_at(vint, 2) == 1);

	v_set_length(vint, 1024);
	assert(v_at(vint, 1023) == NULL);
	v_append(vint, ivals);
	assert(*(int*)v_at(vint, 1024) == 0);

	v_clear(vint);
	/* test for memory leak */
	v_dtor(vint, myfree, NULL);
	int *life = malloc(sizeof *life);
	*life = 42;
	v_append(vint, life);

	v_free(vint);

	return 0;
}
