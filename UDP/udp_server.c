/**************************************************************************************************************************************
*
*   字节序：数据以字节流的方式进行传输，底层都是采用二进制，字节流的顺序是由架构决定的，现在假设使用X86架构，是采用小端存储
*	网络字节序：
*	本地字节序：
*
*	大端存储：低地址存储高字节   当数据超过1个字节的时候才需要区分大端还是小端  假设int型 0x12345678   0x12  | 0x34  | 0x56 |  0x78
*	小端存储：低地址存储低字节   当数据超过1个字节的时候才需要区分大端还是小端  假设int型 0x12345678   0x78  | 0x56  | 0x34 |  0x12
*
*	设备A采用X86架构，所以设备A采用小端存储  待发送的数据 ：0x12345678
*   设备B采用ARM架构，所以设备B采用大端存储  待接收的数据 ：？
*
*   为了统一发送数据的格式，所以互联网传输的数据统一采用大端方式，为了方便开发，linux系统提供了转换的接口：htonl、htons、ntohl、ntohs
*
*   h :host  主机/本地
*   to:      转换
*   n :net   网络
*   l :long  长整型
*   s :short 短整型
*
*   htons:把本地字节序的一个短整型转换为网络字节序
*   htonl:把本地字节序的一个长整型转换为网络字节序
*   ntohs:把网络字节序的一个短整型转换为本地字节序
*   ntohl:把网络字节序的一个长整型转换为本地字节序
* ***********************************************************************************************************************************/
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

int main(int argc,char *argv[])
{
	//检查参数有效性

	

	//1.创建UDP套接字             
	int udp_socket = socket(AF_INET, SOCK_DGRAM, 0);
	if (udp_socket == -1)
	{
		fprintf(stderr, "udp socket error,errno:%d,%s\n",errno,strerror(errno));
		exit(1);
	}

	//2.需要先绑定主机的端口和地址
	struct sockaddr_in  host_addr;

	host_addr.sin_family 		= AF_INET; 						//协议族，是固定的
	host_addr.sin_port   		= htons(atoi(argv[1]));			//目标端口，必须转换为网络字节序
	host_addr.sin_addr.s_addr = inet_addr(argv[2]);				//目标地址 "192.168.64.xxx"  已经转换为网络字节序
	
	bind(udp_socket,(struct sockaddr *)&host_addr, sizeof(host_addr));

	//3.循环调用recvfrom准备接收数据
	char buf[128] = {0};
	while(1)
	{
		recvfrom(udp_socket,buf,sizeof(buf), 0 ,NULL,NULL);
		printf("recv data is [%s]\n",buf);
		bzero(buf,sizeof(buf));
	}
	
	// char buf[128] = {0};
	// struct sockaddr_in client;
	// socklen_t client_len = sizeof(client);
	// while(1)
	// {
	// 	recvfrom(udp_socket,buf,sizeof(buf), 0 ,&client,&client_len);//默认会阻塞
	// 	printf("client ip is [%s],recv data is [%s]\n",inet_ntoa(client.sin_addr),buf);
	// 	bzero(buf,sizeof(buf));
	// }

	return 0;

}
