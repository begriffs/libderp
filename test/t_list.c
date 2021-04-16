#include "list.h"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#define ARRAY_LEN(a) (sizeof(a)/sizeof(*a))

int cmpint(const void *a, const void *b)
{
	return *(int*)a - *(int*)b;
}

int main(void)
{
	int ivals[] =  {0,1,2,3,4,5,6,7,8,9};
	size_t i;
	list *lint = l_new(NULL);
	assert(l_length(lint) == 0);
	assert(l_is_empty(lint));

	for (i = 0; i < ARRAY_LEN(ivals); i++)
		l_prepend(lint, ivals+i);
	assert(l_length(lint) == ARRAY_LEN(ivals));
	assert(*(int*)l_first(lint)->data == 9);
	assert(*(int*)l_remove_first(lint)->data == 9);
	assert(*(int*)l_remove_first(lint)->data == 8);
	l_sort(lint, cmpint);
	assert(*(int*)l_first(lint)->data == 0);
	assert(*(int*)l_first(lint)->next->data == 1);
	assert(*(int*)l_first(lint)->next->next->data == 2);
	assert(*(int*)l_first(lint)->next->next->next->data == 3);

	l_clear(lint);
	assert(l_length(lint) == 0);
	for (i = 0; i < ARRAY_LEN(ivals); i++)
		l_append(lint, ivals+i);
	assert(l_length(lint) == ARRAY_LEN(ivals));
	assert(*(int*)l_last(lint)->data == 9);
	assert(*(int*)l_remove_last(lint)->data == 9);
	assert(*(int*)l_remove_last(lint)->data == 8);

	l_free(lint);

	return 0;
}
