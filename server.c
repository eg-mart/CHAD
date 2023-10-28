#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>

#define PORT 5002
#define ADRESS "89.223.123.237"
#define MAX_COUNT_SOCKS 100
#define MAX_BUF 1024

int open_server_socket(int *server_sock);
int send_to_all(int socks[], size_t socks_num, char *msg);
int recv_messages(int socks[], size_t socks_num, char *msg);
int run_server(int server_sock);
int remove_closed_sockets(int socks[], size_t socks_num);
char *get_socket_name(int sock);

int main()
{
	int server_sock = 0;
	open_server_socket(&server_sock);
	run_server(server_sock);
	close(server_sock);

    return 0;
}

int run_server(int server_sock)
{
	listen(server_sock, 1);

	size_t i;
	int client_socks[MAX_COUNT_SOCKS] = {};
	for (i = 0; i < MAX_COUNT_SOCKS; i++)
		client_socks[i] = -1;
	int client_socks_num = 0;
	while (1) {
		client_socks[client_socks_num] = accept(server_sock, NULL, NULL);
		if (client_socks[client_socks_num] > 0) {
			fcntl(client_socks[client_socks_num], F_SETFL, O_NONBLOCK);
			client_socks_num++;
		}

		char msg_buf[MAX_BUF] = "";
		int recv_status = recv_messages(client_socks, client_socks_num, msg_buf);
		if(recv_status == 0) {
			printf("message: %s\n", msg_buf);
			send_to_all(client_socks, client_socks_num, msg_buf);
		}

		remove_closed_sockets(client_socks, client_socks_num);
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

int send_to_all(int socks[], size_t socks_num, char *msg)
{
	int i;
	for (i = 0; i < socks_num; i++) {
		if (socks[i] == -1) continue;
		send(socks[i], msg, strlen(msg), 0);
	}
	return 0;
}
	
int recv_messages(int socks[], size_t socks_num, char *msg)
{
	int i;
	char tmp_buf[MAX_BUF] = {};
	const int MAX_IP_LEN = 16;
    for (i = 0; i < socks_num; i++) {
        if (socks[i] == -1) continue;
        int bytes_read = recv(socks[i], tmp_buf, MAX_BUF - MAX_IP_LEN, 0);
        if (bytes_read > 0) {
        	strcpy(msg, "[");
        	strncat(msg, get_socket_name(socks[i]), MAX_IP_LEN);
        	strcat(msg, "] ");
        	strncat(msg, tmp_buf, MAX_BUF - MAX_IP_LEN);
            return 0;
        }
    }
    return 1;
}

int remove_closed_sockets(int socks[], size_t socks_num)
{
	int i;
	for (i = 0; i < socks_num; i++) {
		int status = recv(socks[i], NULL, 1, MSG_PEEK | MSG_DONTWAIT);
		if (status == 0) {
			close(socks[i]);
			socks[i] = -1;
		}
	}
	return 0;
}

char *get_socket_name(int sock)
{
	struct sockaddr_in ip;
	socklen_t ip_len = sizeof(struct sockaddr_in);
	getpeername(sock, (struct sockaddr*) &ip, &ip_len);
	return inet_ntoa(ip.sin_addr);
}
