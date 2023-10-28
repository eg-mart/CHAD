#ifndef SOCKETS_H
#define SOCKETS_H

#include "list.h"

#define PORT 5002
#define ADRESS "89.223.123.237"
#define MAX_COUNT_SOCKS 100
#define MAX_BUF 1024

int open_server_socket(int *server_sock);
int send_to_all(List *socks, char *msg);
int recv_messages(List *socks, char *msg);
int run_server(int server_sock);
List *remove_closed_sockets(List *socks);
char *get_socket_name(int sock);

#endif
