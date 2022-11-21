#include "internal/alloc.h"
#include "derp/list.h"
#include "derp/treemap.h"

/* AA (Arne Andersson) Tree:
 * Balanced Search Trees Made Simple
 * https://user.it.uu.se/~arnea/ps/simp.pdf
 *
 * As fast as an RB-tree, but free of those
 * ugly special cases. */

struct tm_node
{
	int level;
	struct map_pair *pair;
	struct tm_node *left, *right;
};

struct treemap
{
	struct tm_node *root, *bottom;
	struct tm_node *deleted, *last;

	dtor *key_dtor;
	dtor *val_dtor;
	comparator *cmp;
	void *cmp_aux;
	void *dtor_aux;
};

struct tm_iter
{
	list *stack;
	struct tm_node *n, *bottom;
};

treemap *
tm_new(comparator *cmp, void *cmp_aux)
{
	treemap *t = internal_malloc(sizeof *t);
	struct tm_node *bottom = internal_malloc(sizeof *bottom);
	if (!t || !bottom)
	{
		internal_free(t);
		internal_free(bottom);
		return NULL;
	}
	/* sentinel living below all leaves */
	*bottom = (struct tm_node){
		.left = bottom, .right = bottom, .level = 0
	};
	*t = (treemap){
		.root = bottom,
		.bottom = bottom,
		.deleted = bottom,
		.last = bottom,
		.cmp = cmp,
		.cmp_aux = cmp_aux
	};
	return t;
}

void
tm_free(treemap *t)
{
	if (!t)
		return;
	tm_clear(t);
	internal_free(t->bottom);
	internal_free(t);
}

void
tm_dtor(treemap *t, dtor *key_dtor, dtor *val_dtor, void *dtor_aux)
{
	if (!t)
		return;
	t->key_dtor = key_dtor;
	t->val_dtor = val_dtor;
	t->dtor_aux = dtor_aux;
}

static size_t
internal_tm_length(const struct tm_node *n,
                   const struct tm_node *bottom)
{
	if (n == bottom)
		return 0;
	return 1 +
		internal_tm_length(n->left, bottom) +
		internal_tm_length(n->right, bottom);
}

size_t
tm_length(const treemap *t)
{
	return t ? internal_tm_length(t->root, t->bottom) : 0;
}

bool
tm_is_empty(const treemap *t)
{
	return tm_length(t) == 0;
}

static void *
internal_tm_at(const treemap *t, const struct tm_node *n,
               const void *key)
{
	if (n == t->bottom)
		return NULL;
	int x = t->cmp(key, n->pair->k, t->cmp_aux);
	if (x == 0)
		return n->pair->v;
	else if (x < 0)
		return internal_tm_at(t, n->left, key);
	return internal_tm_at(t, n->right, key);
}

void *
tm_at(const treemap *t, const void *key)
{
	return t ? internal_tm_at(t, t->root, key) : NULL;
}

static struct tm_node *
internal_tm_skew(struct tm_node *n) {
	if (n->level != n->left->level)
		return n;
	struct tm_node *left = n->left;
	n->left = left->right;
	left->right = n;
	n = left;
	return n;
}

static struct tm_node *
internal_tm_split(struct tm_node *n) {
	if(n->right->right->level != n->level)
		return n;
	struct tm_node *right = n->right;
	n->right = right->left;
	right->left = n;
	n = right;
	n->level++;
	return n;
}

static struct tm_node *
internal_tm_insert(treemap *t, struct tm_node *n,
                   struct tm_node *prealloc)
{
	if (n == t->bottom)
		return prealloc;
	int x = t->cmp(prealloc->pair->k, n->pair->k, t->cmp_aux);
	if (x < 0)
		n->left = internal_tm_insert(t, n->left, prealloc);
	else if (x > 0)
		n->right = internal_tm_insert(t, n->right, prealloc);
	else
	{
		/* prealloc was for naught, but we'll use its value */
		if (n->pair->v != prealloc->pair->v && t->val_dtor)
			t->val_dtor(n->pair->v, t->dtor_aux);
		if (n->pair->k != prealloc->pair->k && t->key_dtor)
			t->key_dtor(n->pair->k, t->dtor_aux);
		*n->pair = *prealloc->pair;
		internal_free(prealloc->pair);
		internal_free(prealloc);
		return n;
	}
	return internal_tm_split(internal_tm_skew(n));
}

bool
tm_insert(treemap *t, void *key, void *val)
{
	if (!t)
		return false;
	/* attempt the malloc before potentially splitting
	 * and skewing the tree, so the insertion can be a
	 * no-op on failure */
	struct tm_node *prealloc = internal_malloc(sizeof *prealloc);
	struct map_pair *p = internal_malloc(sizeof *p);
	if (!prealloc || !p)
	{
		internal_free(prealloc);
		internal_free(p);
		return false;
	}
	*p = (struct map_pair){.k = key, .v = val};
	*prealloc = (struct tm_node){
		.level = 1, .pair = p, .left = t->bottom, .right = t->bottom
	};

	t->root = internal_tm_insert(t, t->root, prealloc);
	return true;
}

static struct tm_node *
internal_tm_remove(treemap *t, struct tm_node *n, void *key)
{
	if (n == t->bottom)
		return n;

	/* 1: search down the tree and set pointers last and deleted */

	t->last = n;
	if (t->cmp(key, n->pair->k, t->cmp_aux) < 0)
		n->left = internal_tm_remove(t, n->left, key);
	else
	{
		t->deleted = n;
		n->right = internal_tm_remove(t, n->right, key);
	}

	/* 2: At the bottom of the tree, remove element if present */

	if (n == t->last && t->deleted != t->bottom &&
	    t->cmp(key, t->deleted->pair->k, t->cmp_aux) == 0)
	{
		if (t->key_dtor)
			t->key_dtor(t->deleted->pair->k, t->dtor_aux);
		if (t->val_dtor)
			t->val_dtor(t->deleted->pair->v, t->dtor_aux);

		*t->deleted->pair = *n->pair;
		t->deleted = t->bottom;
		n = n->right;

		internal_free(t->last->pair);
		internal_free(t->last);
	} /* 3: on the way back up, rebalance */
	else if (n->left->level  < n->level-1 ||
	         n->right->level < n->level-1) {
		n->level--;
		if (n->right->level > n->level)
			n->right->level = n->level;
		n               = internal_tm_skew(n);
		n->right        = internal_tm_skew(n->right);
		n->right->right = internal_tm_skew(n->right->right);
		n               = internal_tm_split(n);
		n->right        = internal_tm_split(n->right);
	}
	return n;
}

bool
tm_remove(treemap *t, void *key)
{
	if (!t)
		return false;
	t->root = internal_tm_remove(t, t->root, key);
	return true; // TODO: return false if key wasn't found
}

static void
internal_tm_clear(treemap *t, struct tm_node *n)
{
	if (n == t->bottom)
		return;
	internal_tm_clear(t, n->left);
	internal_tm_clear(t, n->right);
	if (t->key_dtor)
		t->key_dtor(n->pair->k, t->dtor_aux);
	if (t->val_dtor)
		t->val_dtor(n->pair->v, t->dtor_aux);
	internal_free(n->pair);
	internal_free(n);
}

void
tm_clear(treemap *t)
{
	if (!t)
		return;
	internal_tm_clear(t, t->root);
	t->root = t->deleted = t->last = t->bottom;
}

tm_iter *
tm_iter_begin(treemap *t)
{
	if (!t)
		return NULL;
	struct tm_iter *i = internal_malloc(sizeof *i);
	list *l = l_new();
	if (!i || !l)
	{
		internal_free(i);
		l_free(l);
		return NULL;
	}
	*i = (struct tm_iter){
		.stack = l,
		.n = t->root,
		.bottom = t->bottom
	};
	return i;
}

struct map_pair *
tm_iter_next(tm_iter *i)
{
	if (!i)
		return NULL;
	if (l_is_empty(i->stack) && i->n == i->bottom)
		return NULL; /* done */
	if (i->n != i->bottom)
	{
		if (!l_append(i->stack, i->n))
			return NULL; /* OOM */
		i->n = i->n->left;
		return tm_iter_next(i);
	}
	struct tm_node *result = l_remove_last(i->stack);
	i->n = result->right;
	return result->pair;
}

void
tm_iter_free(tm_iter *i)
{
	if (i)
		l_free(i->stack);
	internal_free(i);
}
