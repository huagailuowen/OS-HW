// server.c
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>

int main() {
    int server_fd, client_fd;
    struct sockaddr_in addr;
    char buf[1024] = {0};

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(12345);
    addr.sin_addr.s_addr = INADDR_ANY;

    bind(server_fd, (struct sockaddr *)&addr, sizeof(addr));
    listen(server_fd, 1);

    client_fd = accept(server_fd, NULL, NULL);
    recv(client_fd, buf, sizeof(buf), 0);
    printf("Received: %s\n", buf);
    close(client_fd);
    close(server_fd);
    return 0;
}
