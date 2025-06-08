// client.c
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>
#include <sys/socket.h>

#define MAX_SOCKETS 20  // 尝试创建的socket数量
#define SERVER_PORT 12345

int main() {
    int sockets[MAX_SOCKETS];
    struct sockaddr_in addr;
    int connected_count = 0;
    int refused_count = 0;
    
    // 初始化socket数组
    for (int i = 0; i < MAX_SOCKETS; i++) {
        sockets[i] = -1;
    }
    
    // 设置服务器地址
    addr.sin_family = AF_INET;
    addr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);

    printf("Testing socket limits - attempting to create %d sockets...\n", MAX_SOCKETS);
    
    for (int i = 0; i < MAX_SOCKETS; i++) {
        printf("\n--- Attempt %d ---\n", i + 1);
        
        // 创建socket
        sockets[i] = socket(AF_INET, SOCK_STREAM, 0);
        if (sockets[i] < 0) {
            printf("Socket creation failed: %s (errno: %d)\n", strerror(errno), errno);
            refused_count++;
            continue;
        }
        
        printf("Socket %d created (fd=%d)\n", i + 1, sockets[i]);
        
        // 尝试连接
        printf("Connecting socket %d to server...\n", i + 1);
        if (connect(sockets[i], (struct sockaddr *)&addr, sizeof(addr)) < 0) {
            printf("Connect failed for socket %d: %s (errno: %d)\n", 
                   i + 1, strerror(errno), errno);
            close(sockets[i]);
            sockets[i] = -1;
            refused_count++;
            continue;
        }
        
        printf("Socket %d connected successfully\n", i + 1);
        connected_count++;
        
        // 发送测试数据
        char message[100];
        snprintf(message, sizeof(message), "Message from socket %d", i + 1);
        
        int bytes_sent = send(sockets[i], message, strlen(message), 0);
        if (bytes_sent < 0) {
            printf("Send failed for socket %d: %s (errno: %d)\n", 
                   i + 1, strerror(errno), errno);
            if (errno == EAGAIN) {
                printf("*** SOCKET LIMIT REACHED at socket %d ***\n", i + 1);
                refused_count++;
                // 继续尝试其他socket以确认限制持续生效
            }
        } else {
            printf("Sent %d bytes on socket %d\n", bytes_sent, i + 1);
        }
        
        // 短暂延迟，观察内核日志
        sleep(1);
    }
    
    printf("\n=== Test Results ===\n");
    printf("Successfully connected: %d sockets\n", connected_count);
    printf("Failed/Refused: %d attempts\n", refused_count);
    printf("Total attempts: %d\n", MAX_SOCKETS);
    
    // 等待一段时间保持连接
    printf("\nKeeping connections alive for 5 seconds...\n");
    sleep(5);
    
    // 关闭所有socket
    printf("\nClosing all sockets...\n");
    for (int i = 0; i < MAX_SOCKETS; i++) {
        if (sockets[i] != -1) {
            printf("Closing socket %d (fd=%d)\n", i + 1, sockets[i]);
            close(sockets[i]);
        }
    }
    
    printf("All sockets closed. Test completed.\n");
    return 0;
}
