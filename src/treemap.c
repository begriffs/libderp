#include "treemap.h"

#include <stdlib.h>

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

treemap *
tm_new(comparator *cmp, void *cmp_aux)
{
	treemap *t = malloc(sizeof *t);
	struct tm_node *bottom = malloc(sizeof *bottom);
	if (!t || !bottom)
	{
		free(t);
		free(bottom);
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
	free(t->bottom);
	free(t);
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
_tm_length(const struct tm_node *n, const struct tm_node *bottom)
{
	if (n == bottom)
		return 0;
	return 1 +
		_tm_length(n->left, bottom) + _tm_length(n->right, bottom);
}

size_t
tm_length(const treemap *t)
{
	return t ? _tm_length(t->root, t->bottom) : 0;
}

bool
tm_is_empty(const treemap *t)
{
	return tm_length(t) == 0;
}

static void *
_tm_at(const treemap *t, const struct tm_node *n, const void *key)
{
	if (n == t->bottom)
		return NULL;
	int x = t->cmp(key, n->pair->k, t->cmp_aux);
	if (x == 0)
		return n->pair->v;
	else if (x < 0)
		return _tm_at(t, n->left, key);
	return _tm_at(t, n->right, key);
}

void *
tm_at(const treemap *t, const void *key)
{
	return t ? _tm_at(t, t->root, key) : NULL;
}

static struct tm_node *
_tm_skew(struct tm_node *n) {
   if (n->level != n->left->level)
	   return n;
   struct tm_node *left = n->left;
   n->left = left->right;
   left->right = n;
   n = left;
   return n;
}

static struct tm_node *
_tm_split(struct tm_node *n) {
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
_tm_insert(treemap *t, struct tm_node *n, struct tm_node *prealloc)
{
	if (n == t->bottom)
		return prealloc;
	int x = t->cmp(prealloc->pair->k, n->pair->k, t->cmp_aux);
	if (x < 0)
		n->left = _tm_insert(t, n->left, prealloc);
	else if (x > 0)
		n->right = _tm_insert(t, n->right, prealloc);
	else
	{
		/* prealloc was for naught, but we'll use its value */
		if (n->pair->v != prealloc->pair->v && t->val_dtor)
			t->val_dtor(n->pair->v, t->dtor_aux);
		n->pair->v = prealloc->pair->v;
		if (n->pair->k != prealloc->pair->k && t->key_dtor)
			t->key_dtor(prealloc->pair->k, t->dtor_aux);
		free(prealloc->pair);
		free(prealloc);
		return n;
	}
	return _tm_split(_tm_skew(n));
}

bool
tm_insert(treemap *t, void *key, void *val)
{
	if (!t)
		return false;
	/* attempt the malloc before potentially splitting
	 * and skewing the tree, so the insertion can be a
	 * no-op on failure */
	struct tm_node *prealloc = malloc(sizeof *prealloc);
	struct map_pair *p = malloc(sizeof *p);
	if (!prealloc || !p)
	{
		free(prealloc);
		free(p);
		return false;
	}
	*p = (struct map_pair){.k = key, .v = val};
	*prealloc = (struct tm_node){
		.level = 1, .pair = p, .left = t->bottom, .right = t->bottom
	};

	t->root = _tm_insert(t, t->root, prealloc);
	return true;
}

static struct tm_node *
_tm_remove(treemap *t, struct tm_node *n, void *key)
{
	if (n == t->bottom)
		return n;

	/* 1: search down the tree and set pointers last and deleted */

	t->last = n;
	if (t->cmp(key, n->pair->k, t->cmp_aux) < 0)
		n->left = _tm_remove(t, n->left, key);
	else
	{
		t->deleted = n;
		n->right = _tm_remove(t, n->right, key);
	}

	/* 2: At the bottom of the tree, remove element if present */

	if (n == t->last && t->deleted != t->bottom &&
	    t->cmp(key, t->deleted->pair->k, t->cmp_aux) == 0)
	{
		t->deleted->pair->k = n->pair->k;
		t->deleted = t->bottom;
		n = n->right;

		if (t->key_dtor)
			t->key_dtor(t->last->pair->k, t->dtor_aux);
		if (t->val_dtor)
			t->val_dtor(t->last->pair->v, t->dtor_aux);
		free(t->last->pair);
		free(t->last);
	} /* 3: on the way back up, rebalance */
	else if (n->left->level  < n->level-1 ||
	           n->right->level < n->level-1) {
		n->level--;
		if (n->right->level > n->level)
			n->right->level = n->level;
		n               = _tm_skew(n);
		n->right        = _tm_skew(n->right);
		n->right->right = _tm_skew(n->right->right);
		n               = _tm_split(n);
		n->right        = _tm_split(n->right);
	}
	return n;
}

bool
tm_remove(treemap *t, void *key)
{
	if (!t)
		return false;
	t->root = _tm_remove(t, t->root, key);
	return true; // TODO: return false if key wasn't found
}

static void
_tm_clear(treemap *t, struct tm_node *n)
{
	if (n == t->bottom)
		return;
	_tm_clear(t, n->left);
	_tm_clear(t, n->right);
	if (t->key_dtor)
		t->key_dtor(n->pair->k, t->dtor_aux);
	if (t->val_dtor)
		t->val_dtor(n->pair->v, t->dtor_aux);
	free(n->pair);
	free(n);
}

void
tm_clear(treemap *t)
{
	if (!t)
		return;
	_tm_clear(t, t->root);
	t->root = t->deleted = t->last = t->bottom;
}
