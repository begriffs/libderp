#include "derp/common.h"
#include "derp/list.h"

#include <assert.h>
#include <stdlib.h>

#define ARRAY_LEN(a) (sizeof(a)/sizeof(*a))

int cmpint(const void *a, const void *b, void *aux)
{
	(void)aux;
	return *(int*)a - *(int*)b;
}

int ivals[] = {0,1,2,3,4,5,6,7,8,9};

int main(void)
{
	list *l = l_new();

	assert(l_length(l) == 0);
	assert(l_is_empty(l));
	l_prepend(l, &ivals[0]);
	assert(l_length(l) == 1);
	l_prepend(l, &ivals[0]);
	assert(l_length(l) == 2);
	assert(!l_is_empty(l));
	l_remove_first(l);
	l_remove_first(l);
	l_remove_first(l); /* redundant */
	assert(l_is_empty(l));
	l_append(l, &ivals[0]);
	assert(l_length(l) == 1);
	l_remove_first(l);
	assert(l_is_empty(l));

	size_t i;
	for (i = 0; i < 3; i++)
		l_append(l, ivals+i);
	l_remove(l, l_first(l)->next);
	assert(*(int*)l_first(l)->data == 0);
	assert(*(int*)l_first(l)->next->data == 2);

	assert(*(int*)l_last(l)->data == 2);
	assert(*(int*)l_last(l)->prev->data == 0);

	l_insert(l, l_first(l)->next, ivals+8);
	assert(*(int*)l_first(l)->data == 0);
	assert(*(int*)l_first(l)->next->data == 8);
	assert(*(int*)l_first(l)->next->next->data == 2);
	assert(l_length(l) == 3);

	l_insert_after(l, l_first(l), ivals+7);
	assert(*(int*)l_first(l)->data == 0);
	assert(*(int*)l_first(l)->next->data == 7);
	assert(*(int*)l_first(l)->next->next->data == 8);
	assert(*(int*)l_first(l)->next->next->next->data == 2);

	l_sort(l, cmpint, NULL);
	assert(*(int*)l_first(l)->data == 0);
	assert(*(int*)l_first(l)->next->data == 2);
	assert(*(int*)l_first(l)->next->next->data == 7);
	assert(*(int*)l_first(l)->next->next->next->data == 8);

	l_insert_after(l, l_first(l)->next, ivals+9);
	l_sort(l, cmpint, NULL);
	assert(*(int*)l_first(l)->data == 0);
	assert(*(int*)l_first(l)->next->data == 2);
	assert(*(int*)l_first(l)->next->next->data == 7);
	assert(*(int*)l_first(l)->next->next->next->data == 8);
	assert(*(int*)l_first(l)->next->next->next->next->data == 9);

	l_insert_after(l, l_first(l), ivals+4);
	l_insert(l, l_last(l), ivals+4);
	assert(l_find(l, ivals+4, cmpint, NULL) == l_first(l)->next);
	assert(l_find_last(l, ivals+4, cmpint, NULL) == l_last(l)->prev);

	int notfound = 100;
	assert(!l_find(l, &notfound, cmpint, NULL));
	assert(!l_find_last(l, &notfound, cmpint, NULL));

	l_clear(l);

	for (i = 0; i < ARRAY_LEN(ivals); i++)
		l_append(l, ivals+i);
	assert(l_length(l) == ARRAY_LEN(ivals));
	assert(*(int*)l_last(l)->data == 9);
	assert(*(int*)l_remove_last(l) == 9);
	assert(*(int*)l_remove_last(l) == 8);

	l_clear(l);
	l_dtor(l, derp_free, NULL);
	int *life = malloc(sizeof *life);
	*life = 42;
	l_append(l, life);

	l_free(l);

	return 0;
}
