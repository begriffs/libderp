#include <string.h>

#include "internal/alloc.h"
#include "derp/common.h"

int derp_strcmp(const void *a, const void *b, void *aux)
{
	(void)aux;
	return strcmp(a, b);
}

void derp_free(void *a, void *aux)
{
	(void)aux;
	internal_free(a);
}
