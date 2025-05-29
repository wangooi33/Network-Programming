
/**************接收端不必开启广播权限*********************/

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define PORT 6666//整型

int main(int argc, char const *argv[])
{
	//1.创建UDP套接字             
	int udp_socket = socket(AF_INET, SOCK_DGRAM, 0);
	if (udp_socket == -1)
	{
		fprintf(stderr, "udp socket error,errno:%d,%s\n",errno,strerror(errno));
		exit(1);
	}

	
	
	//5.接收
	struct sockaddr_in  self_addr;
	self_addr.sin_family 		= AF_INET; 				//协议族，是固定的
	self_addr.sin_port   		= htons(PORT);			//服务器端口，必须转换为网络字节序
	self_addr.sin_addr.s_addr   = INADDR_ANY;			//监听所有本地IP
    bind(udp_socket,(struct sockaddr *)&self_addr, sizeof(self_addr));
	char buf[128]={0};
	while(1)
	{
		recvfrom(udp_socket,buf,sizeof(buf), 0 ,NULL,NULL);
		printf("recv data is [%s]\n",buf);
		bzero(buf,sizeof(buf));
	}

	close(udp_socket);
	return 0;
}