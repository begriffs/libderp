#include <stdlib.h>
#include <string.h>

#include "internal/alloc.h"
#include "derp/common.h"

int derp_strcmp(const void *a, const void *b, void *aux)
{
	(void)aux;
	return strcmp(a, b);
}

void *(*internal_malloc)(size_t n)           = malloc;
void *(*internal_realloc)(void *p, size_t n) = realloc;
void  (*internal_free)(void *p)              = free;

void derp_use_alloc_funcs(
	void *(*m)(size_t),
	void *(*r)(void *, size_t),
	void  (*f)(void *))
{
	if (m) internal_malloc = m;
	if (r) internal_realloc = r;
	if (f) internal_free = f;
}

void derp_free(void *a, void *aux)
{
	(void)aux;
	internal_free(a);
}
