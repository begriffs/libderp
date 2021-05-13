#include "treemap.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

int ivals[] = {0,1,2,3,4,5,6,7,8,9};

int scmp(const void *a, const void *b, void *aux)
{
	(void)aux;
	return strcmp(a, b);
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

	/* test for memory leak */
	tm_dtor(t, myfree, myfree, NULL);
	char *key = malloc(5);
	int  *val1 = malloc(sizeof *val1),
	     *val2 = malloc(sizeof *val2);
	strcpy(key, "life");
	*val1 = 42;
	*val2 = 13;
	tm_insert(t, key, val1);
	tm_insert(t, key, val2);

	tm_free(t);

	return 0;
}
