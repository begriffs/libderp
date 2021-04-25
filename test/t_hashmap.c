#include "hashmap.h"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int ivals[] = {0,1,2,3,4,5,6,7,8,9};

unsigned long djb2hash(const void *x)
{
	const char *str = x;
	uint_fast32_t hash = 5381;
	int c;

	if (str)
		while ( (c = *str++) )
			hash = hash * 33 + c;
	return hash;
}

int scmp(const void *a, const void *b, void *aux)
{
	(void)aux;
	return strcmp(a, b);
}

int main(void)
{
	hashmap *h = hm_new(0, djb2hash, scmp, NULL);
	assert(hm_length(h) == 0);
	assert(hm_is_empty(h));

	assert(!hm_at(h, "zero"));
	hm_insert(h, "zero", ivals);
	assert(hm_length(h) == 1);
	assert(*(int*)hm_at(h, "zero") == 0);

	hm_insert(h, "one", ivals+1);
	assert(hm_length(h) == 2);
	assert(*(int*)hm_at(h, "zero") == 0);
	assert(*(int*)hm_at(h, "one") == 1);
	assert(!hm_at(h, "flurgle"));

	hm_remove(h, "one");
	assert(!hm_at(h, "one"));

	hm_clear(h);
	assert(hm_length(h) == 0);
	assert(!hm_at(h, "zero"));

	hm_free(h);

	return 0;
}
