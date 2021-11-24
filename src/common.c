#include "derp/common.h"

#include <stdlib.h>
#include <string.h>

int derp_strcmp(const void *a, const void *b, void *aux)
{
	(void)aux;
	return strcmp(a, b);
}

void derp_free(void *a, void *aux)
{
	(void)aux;
	free(a);
}
