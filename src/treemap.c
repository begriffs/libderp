#include "treemap.h"

#include <stdlib.h>

enum color { RED, BLACK };

struct tm_node
{
	struct tm_node *left, *right, *parent;
	enum color color;
	struct map_pair *pair;
};

struct treemap
{
	struct tm_node *root;

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
	if (!t)
		return NULL;
	*t = (treemap){
		.cmp = cmp,
		.cmp_aux = cmp_aux
	};
	return t;
}

void
tm_free(treemap *t)
{

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
_tm_length(const struct tm_node *n)
{
	if (!n)
		return 0;
	return 1 + _tm_length(n->left) + _tm_length(n->right);
}

size_t
tm_length(const treemap *t)
{
	return t ? _tm_length(t->root) : 0;
}

bool
tm_is_empty(const treemap *t)
{
	return tm_length(t) == 0;
}

static void *
_tm_at(const treemap *t, const struct tm_node *n, const void *key)
{
	if (!n)
		return NULL;
	int x = t->cmp(n->pair->k, key, t->cmp_aux);
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

static void
_tm_left_rotate(treemap *t, struct tm_node *x)
{
	struct tm_node *y = x->right;
	x->right = y->left;
	if (y->left)
		y->left->parent = x;
	y->parent = x->parent;
	if (!x->parent)
		t->root = y;
	else if (x == x->parent->left)
		x->parent->left = y;
	else
		x->parent->right = y;
	y->left = x;
	x->parent = y;
}

static void
_tm_right_rotate(treemap *t, struct tm_node *x)
{
	struct tm_node *y = x->left;
	x->left = y->right;
	if (y->right)
		y->right->parent = x;
	y->parent = x->parent;
	if (!x->parent)
		t->root = y;
	else if (x == x->parent->right)
		x->parent->right = y;
	else
		x->parent->left = y;
	y->right = x;
	x->parent = y;
}

static struct tm_node *
_tm_insert(treemap *t, void *key, void *val)
{
	struct tm_node *parent = NULL, *cursor = t->root;
	while (cursor)
	{
		parent = cursor;
		int diff = t->cmp(key, cursor->pair->k, t->cmp_aux);
		if (diff == 0)
		{
			if (val != cursor->pair->v && t->val_dtor)
				t->val_dtor(cursor->pair->v, t->dtor_aux);
			cursor->pair->v = val;
			return cursor;
		}
		else if (diff < 0)
			cursor = cursor->left;
		else
			cursor = cursor->right;
	}

	struct map_pair *p = malloc(sizeof *p);
	struct tm_node *node = malloc(sizeof *node);
	if (!p || !node)
	{
		free(p);
		free(node);
		return NULL;
	}
	*p = (struct map_pair){
		.k = key, .v = val
	};
	*node = (struct tm_node){
		.color = RED, .parent = parent, .pair = p
	};
	if (!parent)
		t->root = node;
	else if (t->cmp(key, parent->pair->k, t->cmp_aux))
		parent->left = node;
	else
		parent->right = node;
	return node;
}

static void
_tm_insert_fixup(treemap *t, struct tm_node *x)
{
	struct tm_node *uncle;
	while (x->parent && x->parent->color == RED)
	{
		if (x->parent->parent->left == x->parent)
		{
			uncle = x->parent->parent->right;
			if (uncle->color == RED)
			{
				x->parent->parent->color = RED;
				x->parent->color = BLACK;
				uncle->color     = BLACK;
				x = x->parent->parent;
			}
			else
			{
				if (x == x->parent->right)
				{
					x = x->parent;
					_tm_left_rotate(t, x);
				}
				x->parent->color = BLACK;
				x->parent->parent->color = RED;
				_tm_right_rotate(t, x->parent->parent);
			}
		}
		else
		{
			uncle = x->parent->parent->left;
			if (uncle->color == RED)
			{
				x->parent->parent->color = RED;
				x->parent->color = BLACK;
				uncle->color     = BLACK;
				x = x->parent->parent;
			}
			else
			{
				if (x == x->parent->left)
				{
					x = x->parent;
					_tm_right_rotate(t, x);
				}
				x->parent->color = BLACK;
				x->parent->parent->color = RED;
				_tm_left_rotate(t, x->parent->parent);
			}
		}
	}
	t->root->color = BLACK;
}

bool
tm_insert(treemap *t, void *key, void *val)
{
	struct tm_node *n = _tm_insert(t, key, val);
	if (!n)
		return false;
	_tm_insert_fixup(t, n);
	return true;
}

bool
tm_remove(treemap *t, void *key)
{

}

void
tm_clear(treemap *t)
{

}
