#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <linux/types.h>
#include <sys/select.h>

#define PORT 6666
#define BACKLOG 5
#define TIMEOUT 10  // 秒

int main() 
{
    int server_fd, conn_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);

    // 1. 创建 socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("socket");
        exit(1);
    }

    // 2. 设置地址复用（套接字属性）
    int reuse = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

    // 3. 绑定
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) 
	{
        perror("bind");
        close(server_fd);
        exit(1);
    }

    // 4. 监听
    if (listen(server_fd, BACKLOG) < 0) 
	{
        perror("listen");
        close(server_fd);
        exit(1);
    }

    printf("Server listening on port %d, waiting for connection...\n", PORT);

    // 5. 使用 select 实现 accept 超时
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(server_fd, &readfds);

    struct timeval timeout;
    timeout.tv_sec = TIMEOUT;
    timeout.tv_usec = 0;

    int ret = select(server_fd + 1, &readfds, NULL, NULL, &timeout);
    if (ret == -1) 
	{
        perror("select");
        close(server_fd);
        exit(1);
    } else if (ret == 0) 
	{
        fprintf(stderr, "Timeout: no client connected within %d seconds.\n", TIMEOUT);
        close(server_fd);
        exit(1);
    }

    // 6. 有连接请求
    conn_fd = accept(server_fd, (struct sockaddr *)&client_addr, &addr_len);
    if (conn_fd < 0) 
	{
        perror("accept");
        close(server_fd);
        exit(1);
    }

    printf("Client connected: %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

    // 7. 设置 recv 超时时间
    struct timeval recv_timeout;
    recv_timeout.tv_sec = TIMEOUT;
    recv_timeout.tv_usec = 0;
    setsockopt(conn_fd, SOL_SOCKET, SO_RCVTIMEO, &recv_timeout, sizeof(recv_timeout));

    // 8. 接收数据
    char buffer[1024];
    int n = recv(conn_fd, buffer, sizeof(buffer) - 1, 0);
    if (n < 0) 
	{
        perror("recv timeout or error");
    } else if (n == 0) 
	{
        printf("Client closed connection.\n");
    } else 
	{
        buffer[n] = '\0';
        printf("Received: %s\n", buffer);
    }

    close(conn_fd);
    close(server_fd);
    return 0;
}
