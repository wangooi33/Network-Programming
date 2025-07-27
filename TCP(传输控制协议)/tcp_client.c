


#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/udp.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
 #include <unistd.h>
//TCP客户端代码   ./xxx   port  IP


int main(int argc, char const *argv[])
{
	//1.创建TCP套接字
	int tcp_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (tcp_socket == -1)
	{
		fprintf(stderr, "tcp socket error,errno:%d,%s\n",errno,strerror(errno));
		exit(1);
	}

	//2.发起连接请求，等待接受服务器接受连接
	struct sockaddr_in  dest_addr;
	dest_addr.sin_family 		= AF_INET; 						//协议族，是固定的
	dest_addr.sin_port   		= htons(atoi(argv[1]));			//服务器端口，必须转换为网络字节序
	dest_addr.sin_addr.s_addr   = inet_addr(argv[2]);			//服务器地址 "192.168.64.xxx"  

	/**************************************************************************************/

	int ret = connect(tcp_socket,(struct sockaddr *)&dest_addr,sizeof(dest_addr));
	if (ret < 0)
	{
		fprintf(stderr, "connect error,errno:%d,%s\n",errno,strerror(errno));
		exit(1);
	}
	char buf[128] = {0};

	//3.说明双方建立连接，此时可以接收数据
	while(1)
	{
		printf("请输入:");
		scanf("%s",buf);
		write(tcp_socket,buf,sizeof(buf));
		//send(tcp_socket,buf,sizeof(buf),0);,当flag=0时，send和write效果相同
		bzero(buf,128);
	}


	return 0;
}