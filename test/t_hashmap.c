#include "hashmap.h"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int ivals[] = {0,1,2,3,4,5,6,7,8,9};

uint_fast32_t djb2hash(const void *x)
{
	const char *str = x;
	uint_fast32_t hash = 5381;
	int c;

	if (str)
		while ( (c = *str++) )
			hash = hash * 33 + c;
	return hash;
}

int scmp(const void *a, const void *b)
{
	return strcmp(a, b);
}

int main(void)
{
	hashmap *h = hm_new(0, djb2hash, scmp, NULL, NULL);
	assert(hm_length(h) == 0);
	assert(hm_is_empty(h));

	hm_free(h);

	return 0;
}
