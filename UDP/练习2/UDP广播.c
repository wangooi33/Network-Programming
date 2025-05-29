/****************************************************************************************
*
*	这是一个测试UDP套接字的广播属性的案例，本程序用于作为UDP服务器，服务器向网络号为
*	192.168.64的C类网络的广播地址192.168.64.255发送消息，测试结果应该为处于该网络的所有
*	主机都会收到相关的数据包。
*
* **************************************************************************************/
/*默认情况下，教师机（发送端）不会收到自己发出的广播数据，因为它没有在接收端绑定广播地址或使用 recvfrom。 */
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char const *argv[])
{
	//1.创建UDP套接字             
	int udp_socket = socket(AF_INET, SOCK_DGRAM, 0);
	if (udp_socket == -1)
	{
		fprintf(stderr, "udp socket error,errno:%d,%s\n",errno,strerror(errno));
		exit(1);
	}

	// //2.获取UDP套接字的广播属性
	// int flag=-1;
	// getsockopt(udp_socket,SOL_SOCKET,SO_BROADCAST,(void *)&flag,&flag);
	// printf("flag = %d\n",flag); // 0

	// //3.设置UDP套接字的广播属性
	// int flag = 1;
	// setsockopt(udp_socket,SOL_SOCKET,SO_BROADCAST,(void *)&flag,sizeof(flag));
	// //4.获取UDP套接字的广播属性
	// int flag = -1;
	// getsockopt(udp_socket,SOL_SOCKET,SO_BROADCAST,(void *)&flag,&flag);
	// printf("flag = %d\n",flag); // 非0

	//2.设置广播权限
	int flag = 1;
	setsockopt(udp_socket,SOL_SOCKET,SO_BROADCAST,(void *)&flag,sizeof(flag));
	
	//3.利用循环每隔5s想广播地址发送数据包
	char buf[128] = "I am teacher,this is test packet";
	
	struct sockaddr_in  dest_addr;
	dest_addr.sin_family 		= AF_INET; 						//协议族，是固定的
	dest_addr.sin_port   		= htons(atoi(argv[1]));			//服务器端口，必须转换为网络字节序
	dest_addr.sin_addr.s_addr   = inet_addr(argv[2]);			//服务器地址 "192.168.64.xxx"  

	while(1)
	{
		sendto(udp_socket,buf,strlen(buf),0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
		sleep(5);
	}
	
	close(udp_socket);
	return 0;
}