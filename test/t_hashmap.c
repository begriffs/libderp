#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "derp/common.h"
#include "derp/hashmap.h"

#ifdef HAVE_BOEHM_GC
#include <gc/leak_detector.h>
#endif

int ivals[] = {0,1,2,3,4,5,6,7,8,9};

unsigned long djb2hash(const void *x)
{
	const char *str = x;
	unsigned long hash = 5381;
	int c;

	if (str)
		while ( (c = *str++) )
			hash = hash * 33 + c;
	return hash;
}

int main(void)
{
#ifdef HAVE_BOEHM_GC
	GC_set_find_leak(1);
	derp_use_alloc_funcs(
		GC_debug_malloc_replacement,
		GC_debug_realloc_replacement, GC_debug_free);
#endif

	hashmap *h = hm_new(0, djb2hash, derp_strcmp, NULL);
	hm_iter *i;
	assert(hm_length(h) == 0);
	assert(hm_is_empty(h));
	i = hm_iter_begin(h);
	assert(!hm_iter_next(i));
	hm_iter_free(i);

	assert(!hm_at(h, "zero"));
	hm_insert(h, "zero", ivals);
	assert(hm_length(h) == 1);
	assert(*(int*)hm_at(h, "zero") == 0);

	/* change it */
	hm_insert(h, "zero", ivals+1);
	assert(hm_length(h) == 1);
	assert(*(int*)hm_at(h, "zero") == 1);
	/* set it back */
	hm_insert(h, "zero", ivals);
	assert(*(int*)hm_at(h, "zero") == 0);

	hm_insert(h, "one", ivals+1);
	assert(hm_length(h) == 2);
	assert(*(int*)hm_at(h, "zero") == 0);
	assert(*(int*)hm_at(h, "one") == 1);
	assert(!hm_at(h, "flurgle"));

	struct map_pair *p;
	int n_keys = 0;
	for (i = hm_iter_begin(h); (p = hm_iter_next(i)); n_keys++)
		assert(strcmp((char*)p->k, "zero") == 0 ||
		       strcmp((char*)p->k, "one") == 0);
	hm_iter_free(i);
	assert(n_keys == 2);

	hm_remove(h, "one");
	assert(!hm_at(h, "one"));

	hm_clear(h);
	assert(hm_length(h) == 0);
	assert(!hm_at(h, "zero"));

	/* test for memory leak */
	hm_dtor(h, derp_free, derp_free, NULL);
	char *key = malloc(5);
	int  *val1 = malloc(sizeof *val1),
	     *val2 = malloc(sizeof *val2);
	strcpy(key, "life");
	*val1 = 42;
	*val2 = 13;
	hm_insert(h, key, val1);
	hm_insert(h, key, val2);

	hm_free(h);

	/* test iterator when hashmap has only one bucket */
	hashmap *h1 = hm_new(1, djb2hash, derp_strcmp, NULL);
	assert(hm_is_empty(h1));
	hm_insert(h1, "zero", ivals);
	hm_insert(h1, "one", ivals+1);
	assert(hm_length(h1) == 2);
	for (n_keys = 0, i = hm_iter_begin(h1); (p = hm_iter_next(i)); n_keys++)
		;
	assert(n_keys == 2);
	hm_iter_free(i);
	hm_free(h1);

#ifdef HAVE_BOEHM_GC
	CHECK_LEAKS();
#endif
	return 0;
}
