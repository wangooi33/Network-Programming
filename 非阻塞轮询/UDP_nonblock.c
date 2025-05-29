#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 6666
#define BUFSIZE 1024

int main() {
    int udp_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (udp_fd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // 设置非阻塞
    int flags = fcntl(udp_fd, F_GETFL, 0);
    if (flags == -1) {
        perror("fcntl get");
        close(udp_fd);
        exit(EXIT_FAILURE);
    }

    if (fcntl(udp_fd, F_SETFL, flags | O_NONBLOCK) == -1) {
        perror("fcntl set O_NONBLOCK");
        close(udp_fd);
        exit(EXIT_FAILURE);
    }

    // 绑定本地地址
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(udp_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind");
        close(udp_fd);
        exit(EXIT_FAILURE);
    }

    printf("UDP server started on port %d (non-blocking)...\n", PORT);

    // 接收数据循环
    while (1) {
        char buffer[BUFSIZE];
        struct sockaddr_in client_addr;
        socklen_t addr_len = sizeof(client_addr);

        int n = recvfrom(udp_fd, buffer, sizeof(buffer) - 1, 0,
                         (struct sockaddr *)&client_addr, &addr_len);

        if (n == -1) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                // 没有数据，继续轮询
                usleep(100000);  // 睡100ms防止CPU过高
                continue;
            } else {
                perror("recvfrom");
                break;
            }
        }

        buffer[n] = '\0';
        printf("Received from %s:%d: %s\n",
               inet_ntoa(client_addr.sin_addr),
               ntohs(client_addr.sin_port),
               buffer);
    }

    close(udp_fd);
    return 0;
}
