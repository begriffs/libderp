#include "hashmap.h"
#include "list.h"

#include <assert.h>
#include <stdlib.h>

#define DEFAULT_CAPACITY 64

struct pair
{
	void *k;
	void *v;
};

struct hashmap
{
	size_t capacity;
	list **buckets;

	void (*key_dtor)(void *);
	void (*val_dtor)(void *);
	uint_fast32_t (*hash)(const void *);
	int (*cmp)(const void *, const void *);
};

hashmap *
hm_new(size_t capacity,
       uint_fast32_t (*hash)(const void *),
       int (*cmp)(const void *, const void *),
       void (*key_dtor)(void *),
       void (*val_dtor)(void *))
{
	if (!hash || !cmp)
		return NULL;
	if (capacity == 0)
		capacity = DEFAULT_CAPACITY;
	hashmap *h = malloc(sizeof *h);
	if (!h)
		goto fail;
	*h = (hashmap){
		.capacity = capacity,
		.buckets = malloc(capacity * sizeof *h->buckets),
		.key_dtor = key_dtor,
		.val_dtor = val_dtor,
		.hash = hash,
		.cmp = cmp
	};
	if (!h->buckets)
		goto fail;

	size_t i;
	for (i = 0; i < capacity; i++)
		h->buckets[i] = NULL; /* in case allocation fails part-way */
	for (i = 0; i < capacity; i++)
		if (!(h->buckets[i] = l_new(NULL))) /* XXX: proper dtor */
			goto fail;
	return h;

fail:
	hm_free(h);
	return NULL;
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
		free(h->buckets);
	}
	free(h);
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

void *
hm_at(const hashmap *h, const void *key)
{
	if (!h)
		return NULL;
	list *bucket = h->buckets[h->hash(key) % h->capacity];
	list_item *li = l_find(bucket, key, h->cmp);
	if (!li)
		return NULL;
	return ((struct pair*)li->data)->v;
}

bool
hm_insert(hashmap *h, void *key, void *val)
{
	if (!h)
		return false;
	list *bucket = h->buckets[h->hash(key) % h->capacity];
	list_item *li = l_find(bucket, key, h->cmp);
	if (li)
	{
		struct pair *p = (struct pair*)li->data;
		if (p->v != val && h->val_dtor)
			h->val_dtor(p->v);
		p->v = val;
		return true;
	}
	else
	{
		struct pair *p = malloc(sizeof *p);
		if (!p)
			return false;
		*p = (struct pair){.k = key, .v = val};
		l_append(bucket, p);
		return true;
	}
}

bool
hm_remove(hashmap *h, void *key)
{
	if (!h)
		return false;
	list *bucket = h->buckets[h->hash(key) % h->capacity];
	list_item *li = l_find(bucket, key, h->cmp);
	if (!li)
		return false;
	l_remove(bucket, li);
	/* XXX: free li and pair */
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
