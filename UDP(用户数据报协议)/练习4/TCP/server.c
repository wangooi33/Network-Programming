#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <fcntl.h>

#define PORT 8888
#define BACKLOG 5
#define BUF_SIZE 1024

void send_file(int client_sock, const char *filename) {
    struct stat st;
    if (stat(filename, &st) < 0) {
        perror("stat");
        send(client_sock, "NOFILE", 6, 0);
        return;
    }

    FILE *fp = fopen(filename, "rb");
    if (!fp) {
        perror("fopen");
        send(client_sock, "NOFILE", 6, 0);
        return;
    }

    // 发送文件大小
    long filesize = st.st_size;
    send(client_sock, &filesize, sizeof(filesize), 0);

    // 发送文件内容
    char buffer[BUF_SIZE];
    int n;
    while ((n = fread(buffer, 1, BUF_SIZE, fp)) > 0) {
        send(client_sock, buffer, n, 0);
    }

    printf("File [%s] sent successfully.\n", filename);
    fclose(fp);
}

int main() {
    int server_sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr));
    listen(server_sock, BACKLOG);
    printf("Server listening on port %d...\n", PORT);

    while (1) {
        int client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &addr_len);
        if (client_sock < 0) continue;

        char filename[256] = {0};
        recv(client_sock, filename, sizeof(filename), 0);
        printf("Client requests file: %s\n", filename);
        send_file(client_sock, filename);
        close(client_sock);
    }

    close(server_sock);
    return 0;
}
