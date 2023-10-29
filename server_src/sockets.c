#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>

#include "sockets.h"

#define PORT 5002
#define ADRESS "89.223.123.237"
#define MAX_COUNT_SOCKS 1000
#define MAX_BUF 5000
#define MAX_IP_LEN 16
#define POISON_NAME "UNKNOWN"

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
		send(sock->client_sock, msg, strlen(msg), 0);
	return 0;
}
	
int recv_messages(List *socks, char *msg)
{
	int i;
	char tmp_buf[MAX_BUF] = {};
	List *sock = NULL;
	for (sock = socks; sock != NULL; sock = sock->next) {
		int bytes_read = recv(sock->client_sock, tmp_buf, MAX_BUF - MAX_IP_LEN, 0);
		if (bytes_read > 0 && strcmp(sock->client_name, POISON_NAME) == 0) {
			strcpy(sock->client_name, tmp_buf); 
			strcpy(msg, "New user added: ");
			strcat(msg, sock->client_name);
			return 0; 
		}
		if (bytes_read > 0) {
			strcpy(msg, "[");
			strncat(msg, get_socket_name(sock->client_sock), MAX_IP_LEN);
			strcat(msg, "] ");
			strcat(msg, sock->client_name);
			strcat(msg, ": ");
			strncat(msg, tmp_buf, MAX_BUF - MAX_IP_LEN);
			return 0;
		}
	}
	return 1;
}

List *remove_closed_sockets(List *socks)
{
	List *sock = socks;
	while (sock != NULL) {
		int status = recv(sock->client_sock, NULL, 1, MSG_PEEK | MSG_DONTWAIT);
		if (status == 0) {
			char rmv_buf[MAX_NAME] = "";
			strcpy(rmv_buf, "User left: ");
			strcat(rmv_buf, sock->client_name);
			printf("%s\n", rmv_buf);
			List *del_sock = sock;
			sock = sock->next;
			close(del_sock->client_sock);
			socks = list_delete(socks, del_sock->client_sock);
			send_to_all(socks, rmv_buf);
			continue;
		}
		sock = sock->next;
	}
	return socks;
}

char *get_socket_name(int sock)
{
	struct sockaddr_in ip;
	socklen_t ip_len = sizeof(struct sockaddr_in);
	getpeername(sock, (struct sockaddr*) &ip, &ip_len);
	return inet_ntoa(ip.sin_addr);
}
