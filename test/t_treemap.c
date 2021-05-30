#include "derp/treemap.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

int ivals[] = {0,1,2,3,4,5,6,7,8,9};

int scmp(const void *a, const void *b, void *aux)
{
	(void)aux;
	return strcmp(a, b);
}

int icmp(const void *a, const void *b, void *aux)
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
	treemap *t = tm_new(scmp, NULL);
	assert(tm_length(t) == 0);
	assert(tm_is_empty(t));

	assert(!tm_at(t, "zero"));
	tm_insert(t, "zero", ivals);
	assert(tm_length(t) == 1);
	assert(*(int*)tm_at(t, "zero") == 0);

	/* change it */
	tm_insert(t, "zero", ivals+1);
	assert(tm_length(t) == 1);
	assert(*(int*)tm_at(t, "zero") == 1);
	/* set it back */
	tm_insert(t, "zero", ivals);
	assert(*(int*)tm_at(t, "zero") == 0);

	tm_insert(t, "one", ivals+1);
	assert(tm_length(t) == 2);
	assert(*(int*)tm_at(t, "zero") == 0);
	assert(*(int*)tm_at(t, "one") == 1);
	assert(!tm_at(t, "flurgle"));

	tm_remove(t, "one");
	assert(!tm_at(t, "one"));

	tm_clear(t);
	assert(tm_length(t) == 0);
	assert(!tm_at(t, "zero"));

	/* iterator should sort */
	tm_insert(t, "d", NULL);
	tm_insert(t, "a", NULL);
	tm_insert(t, "e", NULL);
	tm_insert(t, "c", NULL);
	tm_insert(t, "b", NULL);

	tm_iter *i = tm_iter_begin(t);
	assert(*(char*)tm_iter_next(i)->k == 'a');
	assert(*(char*)tm_iter_next(i)->k == 'b');
	assert(*(char*)tm_iter_next(i)->k == 'c');
	assert(*(char*)tm_iter_next(i)->k == 'd');
	assert(*(char*)tm_iter_next(i)->k == 'e');
	assert(!tm_iter_next(i));
	tm_iter_free(i);
	tm_clear(t);

	/* test for memory leak */
	tm_dtor(t, myfree, myfree, NULL);
	char *key = malloc(5),
		 *dupkey = malloc(5),
		 *otherkey = malloc(3);
	int  *val1 = malloc(sizeof *val1),
	     *val2 = malloc(sizeof *val2),
	     *val3 = malloc(sizeof *val3);
	strcpy(key, "life");
	strcpy(dupkey, "life");
	*val1 = 42;
	*val2 = 13;
	tm_insert(t, key, val1);
	/* will free key, since dupkey is the same */
	tm_insert(t, dupkey, val2);
	/* safe to double-insert same key */
	tm_insert(t, dupkey, val2);

	/* tm_remove should free as needed */
	*val3 = 200;
	strcpy(otherkey, "hi");
	tm_insert(t, otherkey, val3);
	tm_remove(t, otherkey);

	tm_free(t);

	treemap *t2 = tm_new(icmp, NULL);
	/* insert in ascending order, inherently unbalanced,
	 * to exercise split/skew */
	for (size_t i = 0; i < 10; i++)
		tm_insert(t2, ivals+i, ivals+i);
	assert(tm_length(t2) == 10);

	/* all there? */
	for (size_t i = 0; i < 10; i++)
		assert(*(int*)tm_at(t2, ivals+i) == (int)i);

	/* remove odd ones */
	for (size_t i = 1; i < 10; i+=2)
		tm_remove(t2, ivals+i);
	assert(tm_length(t2) == 5);

	/* evens should still be there */
	for (size_t i = 0; i < 10; i+=2)
		assert((int)i == *(int*)tm_at(t2, ivals+i));

	/* remove evens */
	for (size_t i = 0; i < 10; i+=2)
		tm_remove(t2, ivals+i);
	assert(tm_is_empty(t2));

	tm_free(t2);

	return 0;
}
