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

    printf("Server listening on port 12345...\n");
    
    while (1) {
        client_fd = accept(server_fd, NULL, NULL);
        if (client_fd < 0) {
            perror("accept failed");
            continue;
        }
        
        printf("Client connected, fd=%d\n", client_fd);
        
        int bytes_received = recv(client_fd, buf, sizeof(buf), 0);
        if (bytes_received > 0) {
            buf[bytes_received] = '\0';  // Null-terminate
            printf("Received %d bytes: %s\n", bytes_received, buf);
            
            // 发送回复确认收到数据
            const char *reply = "ACK";
            send(client_fd, reply, strlen(reply), 0);
        } else if (bytes_received == 0) {
            printf("Client disconnected\n");
        } else {
            perror("recv failed");
        }
        
        // 保持连接打开一段时间，让客户端有时间处理
        sleep(2);
        
        close(client_fd);
        printf("Connection closed\n");
        
        // 继续处理下一个连接，支持多轮测试
    }
    
    close(server_fd);
    return 0;
}
