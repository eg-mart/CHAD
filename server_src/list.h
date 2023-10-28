#ifndef LIST_H
#define LIST_H

typedef struct _List {
	int x;
	struct _List *next;
} List ;

List *list_insert(List *head, int x);
List *list_delete(List *head, int x);
int list_count(List *head);

#endif
