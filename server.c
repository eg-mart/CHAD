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
#define POISON_ACCEPT -1

int main()
{
    int listener = socket(AF_INET, SOCK_STREAM, 0);
    fcntl(listener, F_SETFL, O_NONBLOCK);

    if (listener < 0) {
        perror("socket");
        return 1;
    }
        
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(listener, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("bind");
        return 2;
    }

    listen(listener, 1);

    int sock[MAX_COUNT_SOCKS] = {};
    int number_sock = 0;
    int count_valid_sock = 0;
    while (1) {
        sock[count_valid_sock] = accept(listener, NULL, NULL);
        if (sock[count_valid_sock] > 0) {
            fcntl(sock[count_valid_sock], F_SETFL, O_NONBLOCK);
            count_valid_sock++;
        }

        int bytes_read = -1;
        char buf[MAX_BUF] = "";
        for (number_sock = 0; number_sock < count_valid_sock; number_sock++) {
            if (sock[number_sock] == -1) continue;
            bytes_read = recv(sock[number_sock], buf, MAX_BUF, 0);
            if (bytes_read == 0) {
                sock[number_sock] = -1;
            }
            if (bytes_read > 0) { 
                buf[bytes_read] = '\0'; 
                printf("message: %s\n", buf);
                break;
            }
        }

        if (bytes_read < 0) continue;
         
        for (number_sock = 0; number_sock < count_valid_sock; number_sock++) {
            if (sock[number_sock] == -1) continue;
            send(sock[number_sock], buf, bytes_read, 0);
        }
    }
    for (number_sock = 0; number_sock < count_valid_sock; number_sock++)
        close(sock[number_sock]);
    return 0;
}