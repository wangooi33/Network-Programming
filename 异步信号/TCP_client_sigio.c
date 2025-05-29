#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main() {
    int sockfd;
    struct sockaddr_in serv_addr;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(8888);
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

    char msg[1024];
    while (1) {
        printf("Enter message: ");
        fgets(msg, sizeof(msg), stdin);
        send(sockfd, msg, strlen(msg), 0);
    }

    close(sockfd);
    return 0;
}
