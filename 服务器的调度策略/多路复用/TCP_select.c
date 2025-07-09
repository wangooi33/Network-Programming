//select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout)
//超时时间填的是NULL，表示永远阻塞，填的是0，表示不阻塞。第三个参数，如果套接字异常了，使用

/* select函数返回值：
 *	大于0：有事发生，可以使用FD_ISSET来检查哪个文件描述符发生了事件
 *	等于0：超时，无事发生
 * 	小于0：出错 
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>

#define PORT 8888
#define MAX_CLIENTS  FD_SETSIZE //1024
#define BUFFER_SIZE 1024

int main() 
{
    int server_fd, client_fd, max_fd, i;
    int client_sockets[MAX_CLIENTS];
    struct sockaddr_in server_addr, client_addr;
    fd_set read_fds, all_fds;

    socklen_t addrlen;
    char buffer[BUFFER_SIZE];

    // 1. 创建监听套接字
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
	{
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // 2. 绑定地址和端口
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family      = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port        = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) 
	{
        perror("bind");
        exit(EXIT_FAILURE);
    }

    // 3. 开始监听
    if (listen(server_fd, 10) < 0) 
	{
        perror("listen");
        exit(EXIT_FAILURE);
    }

    // 初始化客户端数组
    for (i = 0; i < MAX_CLIENTS; i++) 
	{
        client_sockets[i] = -1;
    }

    FD_ZERO(&all_fds);
    FD_SET(server_fd, &all_fds);
    max_fd = server_fd;

    printf("Server listening on port %d...\n", PORT);

    while (1) 
	{
        read_fds = all_fds;

		/
        int activity = select(max_fd + 1, &read_fds, NULL, NULL, NULL);
        if (activity < 0) 
		{
            perror("select");
            break;
        }

        // 4. 有新连接请求
        if (FD_ISSET(server_fd, &read_fds)) //判断描述符是否还在集合内
		{
            addrlen = sizeof(client_addr);
            client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &addrlen);
            if (client_fd < 0) 
			{
                perror("accept");
                continue;
            }

            printf("New connection from %s:%d\n",
                   inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

            // 添加新客户端
            for (i = 0; i < MAX_CLIENTS; i++) 
			{
                if (client_sockets[i] == -1) 
				{
                    client_sockets[i] = client_fd;
                    FD_SET(client_fd, &all_fds);
                    if (client_fd > max_fd)
                        max_fd = client_fd;
                    break;
                }
            }

            if (i == MAX_CLIENTS) 
			{
                printf("Too many connections\n");
                close(client_fd);
            }
        }

        // 5. 检查各个客户端是否有数据
        for (i = 0; i < MAX_CLIENTS; i++) 
		{
            int sockfd = client_sockets[i];
            if (sockfd != -1 && FD_ISSET(sockfd, &read_fds)) 
			{
                int bytes = recv(sockfd, buffer, sizeof(buffer) - 1, 0);
                if (bytes <= 0) 
				{
                    printf("Client disconnected: fd=%d\n", sockfd);
                    close(sockfd);
                    FD_CLR(sockfd, &all_fds);
                    client_sockets[i] = -1;
                } else {
                    buffer[bytes] = '\0';
                    printf("Received from fd %d: %s", sockfd, buffer);
                    send(sockfd, buffer, bytes, 0);  // 回显
                }
            }
        }
    }

    close(server_fd);
    return 0;
}
