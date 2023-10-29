#include <stdlib.h>
#include <stdio.h>

#include "list.h"

#define PIOSON_NAME "UNKNOWN"

List *list_insert(List *head, int client_sock)
{
	List *new = (List *) malloc(sizeof(List));
	if (!new) return NULL;
	*new = (List) {client_sock, PIOSON_NAME, head};
	return new;
}

List *list_delete(List *head, int client_sock)
{
	if (!head) return NULL;
	if (head->client_sock == client_sock) {
		List *new = head->next;
		free(head);
		return new;
	}
	head->next = list_delete(head->next, client_sock);
	return head;
}

int list_count(List *head)
{
	if (!head) return 0;
	return list_count(head->next) + 1;
}

void list_print(List *head)
{
	if (!head) {
		printf("\n");
		return;
	}
	printf("%d ", head->client_sock);
	list_print(head->next);
}