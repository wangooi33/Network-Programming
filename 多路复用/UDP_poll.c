#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <poll.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 6666
#define BUF_SIZE 1024

int main() {
    int udp_sock;
    struct sockaddr_in serv_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    char buf[BUF_SIZE];

    // 1. 创建 UDP 套接字
    udp_sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (udp_sock < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // 2. 设置为非阻塞
    int flags = fcntl(udp_sock, F_GETFL, 0);
    if (flags < 0) {
        perror("fcntl get");
        exit(EXIT_FAILURE);
    }
    if (fcntl(udp_sock, F_SETFL, flags | O_NONBLOCK) < 0) {
        perror("fcntl set");
        exit(EXIT_FAILURE);
    }

    // 3. 绑定地址
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(udp_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    printf("UDP server using poll started on port %d\n", PORT);

    // 4. 设置 poll 结构体
    struct pollfd fds[1];
    fds[0].fd = udp_sock;
    fds[0].events = POLLIN;

    // 5. 主循环
    while (1) {
        int ret = poll(fds, 1, -1);  // -1 表示阻塞等待
        if (ret == -1) {
            perror("poll");
            continue;
        }

        if (fds[0].revents & POLLIN) {
            int n = recvfrom(udp_sock, buf, BUF_SIZE - 1, 0,
                             (struct sockaddr *)&client_addr, &client_len);
            if (n > 0) {
                buf[n] = '\0';
                printf("Received from %s:%d: %s",
                       inet_ntoa(client_addr.sin_addr),
                       ntohs(client_addr.sin_port),
                       buf);

                // 回显给客户端
                sendto(udp_sock, buf, n, 0,
                       (struct sockaddr *)&client_addr, client_len);
            }
        }
    }

    close(udp_sock);
    return 0;
}
