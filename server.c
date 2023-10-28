#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>

#define PORT 5002
#define ADRESS "89.223.123.237"
#define MAX_COUNT_SOCKS 100
#define MAX_BUF 1024

typedef struct _List {
  int x;
  struct _List *next;
} List ;

List *list_insert(List *head, int x);
List *list_delete(List *head, int x);
int list_count(List *head);

int open_server_socket(int *server_sock);
int send_to_all(List *socks, char *msg);
int recv_messages(List *socks, char *msg);
int run_server(int server_sock);
List *remove_closed_sockets(List *socks);

int main()
{
	int server_sock = 0;
	open_server_socket(&server_sock);
	run_server(server_sock);
	close(server_sock);

    return 0;
}

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

int run_server(int server_sock)
{
	listen(server_sock, 1);

    List *client_socks = NULL;
	while (1) {
        int new_sock = accept(server_sock, NULL, NULL);
		if (new_sock > 0) {
			fcntl(new_sock, F_SETFL, O_NONBLOCK);
            client_socks = list_insert(client_socks, new_sock);
		}

		char msg_buf[MAX_BUF] = "";
		int recv_status = recv_messages(client_socks, msg_buf);
		if(recv_status == 0) {
			printf("message: %s\n", msg_buf);
			send_to_all(client_socks, msg_buf);
		}

		client_socks = remove_closed_sockets(client_socks);
	}
}

int open_server_socket(int *server_sock)
{
    *server_sock = socket(AF_INET, SOCK_STREAM, 0);
    fcntl(*server_sock, F_SETFL, O_NONBLOCK);

    if (*server_sock < 0) {
        perror("socket");
        return 1;
    }
        
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(*server_sock, (struct sockaddr*) &addr, sizeof(addr)) < 0) {
        perror("bind");
        return 2;
    }
    return 0;
}

int send_to_all(List *socks, char *msg)
{
	List *sock = NULL;
	for (sock = socks; sock != NULL; sock = sock->next)
		send(sock->x, msg, strlen(msg), 0);
	return 0;
}
	
int recv_messages(List *socks, char *msg)
{
	List *sock = NULL;
	for (sock = socks; sock != NULL; sock = sock->next) {
        int bytes_read = recv(sock->x, msg, MAX_BUF - 1, 0);
        if (bytes_read > 0) {
            msg[bytes_read] = '\0';
            return 0;
        }
    }
    return 1;
}

List *remove_closed_sockets(List *socks)
{
    List *sock = socks;
	while (sock != NULL) {
		int status = recv(sock->x, NULL, 1, MSG_PEEK | MSG_DONTWAIT);
		if (status == 0) {
            List *del_sock = sock;
            sock = sock->next;
			close(del_sock->x);
			socks = list_delete(socks, del_sock->x);
            continue;
		}
        sock = sock->next;
	}
	return socks;
}