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

#define PORT 80
#define IP "116.62.81.138"
#define KEY "xxx"		//私钥
#define LOCAL "fushun"  //地区


int main(int argc, char const *argv[])
{
	//1.创建TCP套接字
	int tcp_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (tcp_socket == -1)
	{
		fprintf(stderr, "tcp socket error,errno:%d,%s\n",errno,strerror(errno));
		exit(1);
	}

	
	//4.发起连接请求，等待接受服务器接受连接
	struct sockaddr_in  dest_addr;
	dest_addr.sin_family 		= AF_INET; 						//协议族，是固定的
	dest_addr.sin_port   		= htons(PORT);			        //服务器端口，必须转换为网络字节序
	dest_addr.sin_addr.s_addr   = inet_addr(IP);			//服务器地址 "192.168.64.xxx"  


	int ret = connect(tcp_socket,(struct sockaddr *)&dest_addr,sizeof(dest_addr));
	if (ret < 0)
	{
		fprintf(stderr, "connect error,errno:%d,%s\n",errno,strerror(errno));
		exit(1);
	}

    //存储HTTP请求内容：请求行 + 请求字段 + \r\n + 请求包体(可选) + \r\n
    char reqbuf[1024]={0};
    //https://api.seniverse.com/v3/weather/now.json?key=SQqN2t2sb6lurchF2&location=beijing&language=zh-Hans&unit=c
    //请求行: GET URL(统一资源定位符) HTTP/1.1\r\n    （HTTP/1.1，主流版本-> 持久连接）
    //请求字段: HOST:api.seniverse.com
    //\r\n

                                                                                                     //最后要有空格
    sprintf(reqbuf,"GET https://api.seniverse.com/v3/weather/now.json?key=%s&location=%s&language=zh-Hans&unit=c "
                	"HTTP/1.1"
                	"\r\n"
                	"Host:api.seniverse.com\r\n"
                	"\r\n"
            ,KEY,LOCAL);

    
    //发送
    send(tcp_socket,reqbuf,strlen(reqbuf),0);

    //等待服务器响应
    char recvbuf[1024]={0};
    while(1)
    {
        //需要接收两回
        recv(tcp_socket,recvbuf,sizeof(recvbuf),0);//响应头部信息
        printf("%s",recvbuf);
        bzero(recvbuf,1024);
        recv(tcp_socket,recvbuf,sizeof(recvbuf),0);//响应数据内容
        printf("%s\n",recvbuf);
    }
}