#ifndef DERP_ALLOC_H
#define DERP_ALLOC_H

#ifdef DERP_USE_BOEHM_GC

	#include <gc.h>
	#undef  internal_malloc
	#define internal_malloc(n) GC_MALLOC(n)
	#undef  internal_free
	#define internal_free(p) GC_FREE(p)
	#undef  internal_realloc
	#define internal_realloc(p,n) GC_REALLOC(p,n)

#elif !defined(internal_malloc) && \
      !defined(internal_free) && \
      !defined(internal_realloc)

	/* if not otherwise overridden, use stdlib alloc funcs */
	#include <stdlib.h>
	#define internal_malloc(n) malloc(n)
	#define internal_free(p) free(p)
	#define internal_realloc(p,n) realloc(p,n)

#endif

#endif
