#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define PORT 6666
#define BUF_SIZE 1024

int udp_sockfd;  // 全局 socket，用于信号处理函数

void sigio_handler(int signum) {
    char buffer[BUF_SIZE];
    struct sockaddr_in client_addr;
    socklen_t addrlen = sizeof(client_addr);

    int len = recvfrom(udp_sockfd, buffer, sizeof(buffer) - 1, 0,
                       (struct sockaddr *)&client_addr, &addrlen);
    if (len > 0) {
        buffer[len] = '\0';
        printf("📨 收到UDP消息：%s\n", buffer);
    } else {
        perror("recvfrom error");
    }
}

int main() {
    struct sockaddr_in server_addr;

    // 1. 创建 UDP socket
    udp_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (udp_sockfd < 0) {
        perror("socket error");
        exit(1);
    }

    // 2. 绑定端口
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY); // 任意地址
    server_addr.sin_port = htons(PORT);

    if (bind(udp_sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind error");
        exit(2);
    }

    // 3. 设置异步 IO
    fcntl(udp_sockfd, F_SETOWN, getpid());               // 设置 socket 所属进程
    int flags = fcntl(udp_sockfd, F_GETFL);
    fcntl(udp_sockfd, F_SETFL, flags | O_ASYNC);         // 设置 O_ASYNC 标志

    // 4. 设置信号处理函数
    signal(SIGIO, sigio_handler);

    printf("🌐 UDP异步接收服务器已启动，监听端口 %d...\n", PORT);

    // 5. 主线程做其他事，这里就简单 sleep 模拟
    while (1) {
        pause();  // 等待信号触发，节省CPU
    }

    close(udp_sockfd);
    return 0;
}
