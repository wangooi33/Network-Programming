#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <fcntl.h>

#define PORT 8888
#define BUF_SIZE 1024

int main() {
    int sockfd;
    struct sockaddr_in servaddr, cliaddr;
    socklen_t len = sizeof(cliaddr);

    char buffer[BUF_SIZE];
    char *filename = "song.mp3"; // 假设音频文件名

    // 1. 创建 UDP 套接字
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("socket failed");
        exit(1);
    }

    // 2. 绑定本地地址
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(PORT);

    if (bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("bind failed");
        exit(1);
    }

    // 3. 接收客户端请求
    recvfrom(sockfd, buffer, BUF_SIZE, 0, (struct sockaddr *)&cliaddr, &len);
    printf("Client requested: %s\n", buffer);

    // 4. 打开并发送音频文件
    int fd = open(filename, O_RDONLY);
    if (fd < 0) {
        perror("open file failed");
        exit(1);
    }

    ssize_t read_bytes;
    while ((read_bytes = read(fd, buffer, BUF_SIZE)) > 0) {
        sendto(sockfd, buffer, read_bytes, 0, (struct sockaddr *)&cliaddr, len);
        usleep(5000);  // 降低速率避免丢包
    }

    printf("File transfer done.\n");
    close(fd);
    close(sockfd);
    return 0;
}
