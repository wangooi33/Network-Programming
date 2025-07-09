#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <fcntl.h>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8888
#define BUF_SIZE 1024

int main() {
    int sockfd;
    struct sockaddr_in servaddr;
    socklen_t len = sizeof(servaddr);

    char buffer[BUF_SIZE];

    // 1. 创建 UDP 套接字
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("socket failed");
        exit(1);
    }

    // 2. 设置服务器地址
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, SERVER_IP, &servaddr.sin_addr);

    // 3. 发送请求
    char *request = "song.mp3";
    sendto(sockfd, request, strlen(request), 0, (struct sockaddr *)&servaddr, len);

    // 4. 接收音频并写入文件
    int fd = open("recv_song.mp3", O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (fd < 0) {
        perror("file open failed");
        exit(1);
    }

    ssize_t recv_bytes;
    while ((recv_bytes = recvfrom(sockfd, buffer, BUF_SIZE, 0, NULL, NULL)) > 0) {
        write(fd, buffer, recv_bytes);
    }

    printf("Download complete.\n");
    close(fd);
    close(sockfd);

    // 播放音频（Linux）
    system("mpg123 recv_song.mp3");
    return 0;
}
