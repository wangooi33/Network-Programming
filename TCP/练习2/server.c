#define _GNU_SOURCE //为了解决F_SETOWN 未定义标识符 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <sys/types.h>

#define PORT 8888

int conn_fd;  // 全局 socket 描述符用于信号处理

// 🔁 SIGURG 信号处理函数
void sig_urg_handler(int signo) {
    char oob_buf;
    int n = recv(conn_fd, &oob_buf, 1, MSG_OOB);
    if (n > 0) {
        printf("\n[收到 OOB 紧急数据]: %c\n", oob_buf);
    } else {
        perror("recv OOB");
    }
}

int main() {
    int listen_fd;
    struct sockaddr_in serv_addr, cli_addr;
    socklen_t cli_len = sizeof(cli_addr);
    char buf[1024];

    // 1. 创建 TCP socket
    listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd < 0) {
        perror("socket");
        exit(1);
    }

    // 2. 绑定地址
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    bind(listen_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

    // 3. 监听
    listen(listen_fd, 5);
    printf("服务器等待连接...\n");

    // 4. 接受连接
    conn_fd = accept(listen_fd, (struct sockaddr *)&cli_addr, &cli_len);
    if (conn_fd < 0) {
        perror("accept");
        exit(1);
    }
    printf("客户端已连接: %s\n", inet_ntoa(cli_addr.sin_addr));

    // 🔁 5. 设置信号处理器
    signal(SIGURG, sig_urg_handler);

    // 🔁 6. 将当前进程设为 conn_fd 的所有者
    fcntl(conn_fd, F_SETOWN, getpid());

    // 🔁 7. 循环读取普通数据（主流程）
    while (1) {
        int n = recv(conn_fd, buf, sizeof(buf)-1, 0);
        if (n > 0) {
            buf[n] = '\0';
            printf("收到普通数据: %s\n", buf);
        } else if (n == 0) {
            printf("客户端断开连接\n");
            break;
        } else {
            perror("recv normal");
        }
    }

    close(conn_fd);
    close(listen_fd);
    return 0;
}
