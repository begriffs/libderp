#include <assert.h>

#include "internal/alloc.h"
#include "derp/hashmap.h"
#include "derp/list.h"

#define DEFAULT_CAPACITY 64

struct hashmap
{
	size_t capacity;
	list **buckets;

	dtor *key_dtor;
	dtor *val_dtor;
	hashfn *hash;
	comparator *cmp;
	void *cmp_aux;
	void *dtor_aux;
};

struct hm_iter
{
	hashmap *h;
	size_t bucket;
	list_item *offset;
};

static void
internal_hm_free_pair(void *x, void *aux)
{
	hashmap *h = aux;
	struct map_pair *p = x;
	if (h->key_dtor)
		h->key_dtor(p->k, h->dtor_aux);
	if (h->val_dtor)
		h->val_dtor(p->v, h->dtor_aux);
	internal_free(x);
}

hashmap *
hm_new(size_t capacity, hashfn *hash,
       comparator *cmp, void *cmp_aux)
{
	if (!hash || !cmp)
		return NULL;
	if (capacity == 0)
		capacity = DEFAULT_CAPACITY;
	hashmap *h = internal_malloc(sizeof *h);
	if (!h)
		goto fail;
	*h = (hashmap){
		.capacity = capacity,
		.buckets = internal_malloc(capacity * sizeof *h->buckets),
		.hash = hash,
		.cmp = cmp,
		.cmp_aux = cmp_aux
	};
	if (!h->buckets)
		goto fail;

	size_t i;
	for (i = 0; i < capacity; i++)
		h->buckets[i] = NULL; /* in case allocation fails part-way */
	for (i = 0; i < capacity; i++)
	{
		if (!(h->buckets[i] = l_new()))
			goto fail;
		l_dtor(h->buckets[i], internal_hm_free_pair, h);
	}
	return h;

fail:
	hm_free(h);
	return NULL;
}


void
hm_dtor(hashmap *h, dtor *key_dtor, dtor *val_dtor, void *dtor_aux)
{
	if (!h)
		return;
	h->key_dtor = key_dtor;
	h->val_dtor = val_dtor;
	h->dtor_aux = dtor_aux;
}

void
hm_free(hashmap *h)
{
	if (!h)
		return;
	if (h->buckets)
	{
		for (size_t i = 0; i < h->capacity; i++)
			l_free(h->buckets[i]);
		internal_free(h->buckets);
	}
	internal_free(h);
}

size_t
hm_length(const hashmap *h)
{
	if (!h)
		return 0;
	size_t i, n;
	for (n = i = 0; i < h->capacity; i++)
		n += l_length(h->buckets[i]);
	return n;
}

bool
hm_is_empty(const hashmap *h)
{
	return hm_length(h) == 0;
}

/* kinda weird assumption: p is struct pair*, k is type of p->k */
static int
internal_hm_cmp(const void *p, const void *k, void *aux)
{
	assert(p); assert(k); assert(aux);
	hashmap *h = aux;
	return h->cmp(((const struct map_pair *)p)->k, k, h->cmp_aux);
}

void *
hm_at(const hashmap *h, const void *key)
{
	if (!h)
		return NULL;
	list *bucket = h->buckets[h->hash(key) % h->capacity];
	list_item *li = l_find(bucket, key, internal_hm_cmp, (void*)h);
	if (!li)
		return NULL;
	return ((struct map_pair*)li->data)->v;
}

bool
hm_insert(hashmap *h, void *key, void *val)
{
	if (!h)
		return false;
	list *bucket = h->buckets[h->hash(key) % h->capacity];
	list_item *li = l_find(bucket, key, internal_hm_cmp, h);
	if (li)
	{
		struct map_pair *p = (struct map_pair*)li->data;
		if (p->v != val && h->val_dtor)
			h->val_dtor(p->v, h->dtor_aux);
		p->v = val;
	}
	else
	{
		struct map_pair *p = internal_malloc(sizeof *p);
		if (!p)
			return false;
		*p = (struct map_pair){.k = key, .v = val};
		l_append(bucket, p);
	}
	return true;
}

bool
hm_remove(hashmap *h, void *key)
{
	if (!h)
		return false;
	list *bucket = h->buckets[h->hash(key) % h->capacity];
	list_item *li = l_find(bucket, key, internal_hm_cmp, h);
	if (!li)
		return false;
	internal_hm_free_pair(li->data, h);
	l_remove(bucket, li);
	return true;
}

void
hm_clear(hashmap *h)
{
	if (!h)
		return;
	for (size_t i = 0; i < h->capacity; i++)
		l_clear(h->buckets[i]);
}

hm_iter *
hm_iter_begin(hashmap *h)
{
	if (!h)
		return NULL;
	hm_iter *i = internal_malloc(sizeof *i);
	if (!i)
		return NULL;
	*i = (hm_iter){.h = h};
	return i;
}

struct map_pair *
hm_iter_next(hm_iter *i)
{
	if (!i)
		return NULL;
	while (!i->offset && i->bucket < i->h->capacity)
		i->offset = l_first(i->h->buckets[i->bucket++]);
	if (!i->offset)
		return NULL;
	struct map_pair *p = i->offset->data;
	i->offset = i->offset->next;
	return p;
}

void
hm_iter_free(hm_iter *i)
{
	internal_free(i);
}
