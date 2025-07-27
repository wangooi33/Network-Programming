/*********************client************************/

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
#include <pthread.h>

#define SELFIP "192.168.31.10"//字符串
#define DESTIP "192.168.31.20"//字符串
#define PORT 6666//整型

int udp_socket;

//接收线程
void *rcv(void *arg)
{
    char buf[128] = {0};
    struct sockaddr_in sender;
	socklen_t sender_len = sizeof(sender);
	while(1)
	{
		recvfrom(udp_socket,buf,sizeof(buf), 0 ,(struct sockaddr *)&send,&sender_len);//默认会阻塞
		printf("sender ip is [%s],recv data is [%s]\n",inet_ntoa(sender.sin_addr),buf);
		bzero(buf,sizeof(buf));
	}
}
//发送线程
void *send(void *arg)
{
    char buf[128] = {0};
    struct sockaddr_in  dest_addr;
	dest_addr.sin_family 		= AF_INET; 					//协议族，是固定的
	dest_addr.sin_port   		= htons(PORT);		        //目标端口，必须转换为网络字节序
	dest_addr.sin_addr.s_addr   = inet_addr(DESTIP);		//目标地址，已经转换为网络字节序
	while(1)
	{
		scanf("%s",buf);
		sendto(udp_socket,buf,strlen(buf),0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
		bzero(buf,sizeof(buf));
	}
}
int main(int argc,const char argv[])
{
    //1.创建套接字文件
    udp_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (udp_socket == -1)
	{
		fprintf(stderr, "udp socket error,errno:%d,%s\n",errno,strerror(errno));
		exit(1);
	}

    //2.绑定
    struct sockaddr_in  self_addr;
    self_addr.sin_family 		= AF_INET; 						//协议族,是固定的
	self_addr.sin_port   		= htons(PORT);			        //目标端口,必须转换为网络字节序,如果通过scanf输入的话，则需要使用atoi()转换
	self_addr.sin_addr.s_addr   = inet_addr(SELFIP);			//目标地址,已经转换为网络字节序
	//htons()  本地字节序  ->  网络字节序
	//inet_addr() 把点分十进制的主机IP地址转换成网络字节序
	//atoi()  字符串 -> 整型
    bind(udp_socket,(struct sockaddr *)&self_addr, sizeof(self_addr));

    //3.创建线程
    pthread_t rcv_thread;
    pthread_creat(&rcv_thread,NULL,rcv,(void *)&udp_socket);
    pthread_t send_thread;
    pthread_creat(&send_thread,NULL,send,(void *)&udp_socket);

    pthread_join(rcv_thread,NULL);
    pthread_join(send_thread,NULL);

    //4.关闭
    close(udp_socket);
    return 0;
}