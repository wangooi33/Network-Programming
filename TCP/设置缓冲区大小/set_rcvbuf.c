/*请输入希望设置的TCP接收缓冲区大小（字节）：4096
 *系统实际设置的TCP接收缓冲区大小是：8192 字节
 *说明：Linux 会将你设置的值乘以 2 实际应用。*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>

int main() 
{
    int sock;
    int recv_buf_size;

    // 1. 创建 TCP 套接字
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) 
	{
        perror("socket");
        return 1;
    }

    // 2. 输入希望设置的接收缓冲区大小
    printf("请输入希望设置的TCP接收缓冲区大小（字节）：");
    scanf("%d", &recv_buf_size);

    // 3. 使用 setsockopt 设置接收缓冲区大小
    if (setsockopt(sock, SOL_SOCKET, SO_RCVBUF, &recv_buf_size, sizeof(recv_buf_size)) < 0) 
	{
        perror("setsockopt");
        close(sock);
        return 1;
    }

    // 4. 再次用 getsockopt 获取实际的缓冲区大小
    int actual_size = 0;
    socklen_t optlen = sizeof(actual_size);
    if (getsockopt(sock, SOL_SOCKET, SO_RCVBUF, &actual_size, &optlen) < 0) 
	{
        perror("getsockopt");
        close(sock);
        return 1;
    }

    // 5. 输出结果
    printf("系统实际设置的TCP接收缓冲区大小是：%d 字节\n", actual_size);

    // 注意：Linux 内核会自动将你设置的大小乘以2作为默认缓冲区大小
    // 原因是它包括了内核维护的协议开销等内容。

    close(sock);
    return 0;
}
