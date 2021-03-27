#include "vector.h"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#define ARRAY_LEN(a) (sizeof(a)/sizeof(*a))

int cmpint(const void *a, const void *b)
{
	return *(int*)a - *(int*)b;
}

int main(void)
{
	int ivals[] =  {0,1,2,3,4,5,6,7,8,9},
	    ivals2[] = {10,11,12,13,14,15,16,17,18,19};
	size_t i;
	vector *vint = v_new(NULL);
	assert(v_length(vint) == 0);
	assert(v_is_empty(vint));

	for (i = 0; i < ARRAY_LEN(ivals); i++)
		v_prepend(vint, ivals+i);
	assert(v_length(vint) == ARRAY_LEN(ivals));
	assert(*(int*)v_first(vint) == 9);
	assert(*(int*)v_remove_first(vint) == 9);
	assert(*(int*)v_remove_first(vint) == 8);
	v_sort(vint, cmpint);
	assert(*(int*)v_first(vint) == 0);

	v_clear(vint);
	assert(v_length(vint) == 0);
	for (i = 0; i < ARRAY_LEN(ivals); i++)
		v_append(vint, ivals+i);
	assert(v_length(vint) == ARRAY_LEN(ivals));
	assert(*(int*)v_last(vint) == 9);
	assert(*(int*)v_remove_last(vint) == 9);
	assert(*(int*)v_remove_last(vint) == 8);

	assert(v_find_index(vint, ivals+5, cmpint) == 5);
	v_insert(vint, 5, ivals2+5);
	assert(v_find_index(vint, ivals2+5, cmpint) == 5);
	v_swap(vint, 5, 6);
	assert(v_find_index(vint, ivals2+5, cmpint) == 6);
	v_remove(vint, 6);
	assert(v_find_index(vint, ivals2+5, cmpint) == SIZE_MAX);
	assert(v_find_last_index(vint, ivals2+5, cmpint) == SIZE_MAX);

	v_free(vint);

	return 0;
}
