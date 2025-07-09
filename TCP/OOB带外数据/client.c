// client.c - 发送普通数据和 OOB 数据   gcc client.c -o client

/*
 * OOB带外数据每次只能发送一个字节，但是可以发送多次
 * 通过特殊的标志位，让数据到达接收方后可以不受缓冲区和水位线的限制，让接收方可以优先读取
 *
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define PORT 8888
#define SERVER_IP "127.0.0.1"

int main() 
{
    int sock;
    struct sockaddr_in serv_addr;

    // 1. 创建 socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) 
	{
        perror("socket");
        exit(1);
    }

    // 2. 连接服务器
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, SERVER_IP, &serv_addr.sin_addr);

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
	{
        perror("connect");
        exit(1);
    }

    // 3. 发送普通数据
    const char *msg = "Hello, Server!";
    send(sock, msg, strlen(msg), 0);
    printf("已发送普通数据: %s\n", msg);

    sleep(1); // 模拟延时，更好地观察现象

    // 4. 发送 OOB 紧急数据
    char oob = '!';
    send(sock, &oob, 1, MSG_OOB);//标志位为MSG_OOB
    printf("已发送OOB紧急数据: %c\n", oob);

    close(sock);
    return 0;
}
