#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SERVER_IP "127.0.0.1"
#define PORT 8888
#define BUF_SIZE 1024

int main() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_addr;

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr);

    connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr));

    // 输入文件名
    char filename[256];
    printf("Enter filename (e.g., song.mp3): ");
    scanf("%s", filename);

    send(sock, filename, strlen(filename), 0);

    // 接收文件大小
    long filesize;
    int n = recv(sock, &filesize, sizeof(filesize), 0);
    if (n <= 0 || filesize == 0) {
        printf("File not found on server.\n");
        close(sock);
        return 1;
    }

    // 保存为本地文件
    FILE *fp = fopen(filename, "wb");
    if (!fp) {
        perror("fopen");
        close(sock);
        return 1;
    }

    long received = 0;
    char buffer[BUF_SIZE];
    while (received < filesize) {
        int len = recv(sock, buffer, sizeof(buffer), 0);
        if (len <= 0) break;
        fwrite(buffer, 1, len, fp);
        received += len;
    }

    fclose(fp);
    printf("File [%s] downloaded successfully.\n", filename);

    // 可选：调用系统默认播放器播放
    // system("mpg123 song.mp3"); // 需要安装 mpg123 播放器

    close(sock);
    return 0;
}
