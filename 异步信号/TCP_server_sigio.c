#define _GNU_SOURCE //为了解决F_SETOWN 未定义标识符 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

int conn_fd = -1;

void sigio_handler(int signo) {
    char buf[1024] = {0};
    int ret = recv(conn_fd, buf, sizeof(buf)-1, 0);
    if (ret > 0) {
        printf("[SIGIO] Received data: %s\n", buf);
    } else if (ret == 0) {
        printf("[SIGIO] Client closed connection\n");
        close(conn_fd);
        exit(0);
    } else {
        perror("recv error");
    }
}

int main() {
    int listen_fd;
    struct sockaddr_in serv_addr;

    // 1. 创建 socket
    listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd < 0) {
        perror("socket error");
        exit(1);
    }

    // 2. 绑定地址
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(8888);
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(listen_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("bind error");
        exit(1);
    }

    // 3. 监听
    listen(listen_fd, 5);

    printf("Waiting for client...\n");
    conn_fd = accept(listen_fd, NULL, NULL);
    printf("Client connected.\n");

    // 4. 注册 SIGIO 信号处理函数
    signal(SIGIO, sigio_handler);

    // 5. 设置当前进程为 socket 所有者
    fcntl(conn_fd, F_SETOWN, getpid());

    // 6. 设置 FASYNC 标志，开启异步通知
    int flags = fcntl(conn_fd, F_GETFL);
    fcntl(conn_fd, F_SETFL, flags | O_ASYNC);

    // 7. 主进程空转，等待 SIGIO 信号
    while (1) {
        pause(); // 节能等待
    }

    close(conn_fd);
    close(listen_fd);
    return 0;
}
