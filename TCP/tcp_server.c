

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
//TCP服务器代码   ./xxx   port


int main(int argc, char const *argv[])
{
	//1.创建TCP套接字
	int tcp_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (tcp_socket == -1)
	{
		fprintf(stderr, "tcp socket error,errno:%d,%s\n",errno,strerror(errno));
		exit(1);
	}

	//2.绑定自身的IP地址和端口
	struct sockaddr_in  host_addr;

	host_addr.sin_family 		= AF_INET; 						//协议族，是固定的
	host_addr.sin_port   		= htons(atoi(argv[1]));			//目标端口，必须转换为网络字节序
	host_addr.sin_addr.s_addr   = htonl(INADDR_ANY);		    //目标地址  INADDR_ANY 这个宏是一个整数，所以需要使用htonl转换为网络字节序

	bind(tcp_socket,(struct sockaddr *)&host_addr, sizeof(host_addr));

	//3.设置监听  队列最大容量是5
	listen(tcp_socket,5);

	//4.等待接受客户端的连接请求
	struct sockaddr_in  client;
	socklen_t client_len = sizeof(client);

	int connect_fd = accept(tcp_socket,(struct sockaddr *)&client,&client_len); //会阻塞
	char buf[128] = {0};

	//5.说明双方建立连接，此时可以接收数据
	while(1)
	{
		
		
		read(connect_fd,buf,sizeof(buf));
		//recv(connect_fd,buf,sizeof(buf),0);
		printf("recv from [%s],data is = %s\n", inet_ntoa(client.sin_addr) ,buf);
		bzero(buf,sizeof(buf));
	}


	return 0;
}