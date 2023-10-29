#ifndef LIST_H
#define LIST_H

#define MAX_NAME 1000

typedef struct _List {
	int client_sock;
	char client_name[MAX_NAME];
	struct _List *next;
} List ;

List *list_insert(List *head, int client_sock);
List *list_delete(List *head, int client_sock);
void list_print(List *head);
int list_count(List *head);

#endif // LIST_H
