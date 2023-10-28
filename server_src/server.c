#include <unistd.h>

#include "list.h"
#include "sockets.h"

int main()
{
	int server_sock = 0;
	open_server_socket(&server_sock);
	run_server(server_sock);
	close(server_sock);

	return 0;
}

