#include <stdlib.h>

#include "list.h"

List *list_insert(List *head, int x)
{
	List *new = (List *) malloc(sizeof(List));
	if (!new) return NULL;
	*new = (List) {x, head};
	return new;
}

List *list_delete(List *head, int x)
{
	if (!head) return NULL;
	if (head->x == x) {
		List *new = head->next;
		free(head);
		return new;
	}
	return head->next = list_delete(head->next, x);
}

int list_count(List *head)
{
	if (!head) return 0;
	return list_count(head->next) + 1;
}
