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

#include "cJSON.h"

#define  PORT  	80
#define  IPADDR "116.62.81.138"

#define  KEY    "SzIYujfNSBeee1MPr"  //自己的私钥
#define  LOCAL  "beijing"

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
	dest_addr.sin_port   		= htons(PORT);					//服务器端口，必须转换为网络字节序
	dest_addr.sin_addr.s_addr   = inet_addr(IPADDR);			//服务器地址   


	int ret = connect(tcp_socket,(struct sockaddr *)&dest_addr,sizeof(dest_addr));
	if (ret < 0)
	{
		fprintf(stderr, "connect error,errno:%d,%s\n",errno,strerror(errno));
		exit(1);
	}


	//用于存储HTTP的请求内容: 请求行 + 请求字段 + \r\n + 请求包体（可选）  
	char reqbuf[1024] = {0};

	sprintf(reqbuf,"GET https://api.seniverse.com/v3/weather/now.json?key=%s&location=%s&language=en&unit=c "
				   "HTTP/1.1"
				   "\r\n"
				   "Host:api.seniverse.com\r\n"
				   "\r\n"
		    ,KEY,LOCAL);


	//5.说明双方建立连接，此时可以利用HTTP协议发送请求信息，并等待服务器的响应  基于请求/响应
	send(tcp_socket,reqbuf,strlen(reqbuf),0);

	//6.等待服务器的响应
	char recvbuf[1024] = {0};
	
	recv(tcp_socket,recvbuf,sizeof(recvbuf),0);  //第一次返回的响应参数
	printf("%s",recvbuf);
	bzero(recvbuf,1024);

	recv(tcp_socket,recvbuf,sizeof(recvbuf),0);  //第二次返回的响应包体
	printf("%s\n",recvbuf);
	
	//7.对响应包体进行JSON解析
	
	//1) 先把获取的字符串转换为JSON格式
	cJSON * obj = cJSON_Parse(recvbuf);

	//2) 把解析之后的JSON格式进行输出，用于调试
	printf("%s\n",cJSON_Print(obj));

	//3) 对JSON格式进行解析
	cJSON * results = NULL;
	results = cJSON_GetObjectItem(obj, "results");
	printf("array size = %d\n",cJSON_GetArraySize(results));

	//4)对温度进行解析
	cJSON * result = NULL;
	cJSON_ArrayForEach(result, results)
    {
        cJSON * now = cJSON_GetObjectItem(result, "now");
        cJSON * temperature = cJSON_GetObjectItem(now,"temperature");

        printf("temperature : %s\n", temperature->valuestring);

        printf("temperature : %d\n", atoi(temperature->valuestring));
    }

    //8.构造JSON格式
    cJSON *root = cJSON_CreateObject();  //创建一个对象
    printf("%s\n",cJSON_Print(root));

    cJSON *name = NULL;
    name = cJSON_CreateArray();

    cJSON_AddItemToObject(root,"name",name);

    printf("%s\n",cJSON_Print(root));

    cJSON *obj1 = cJSON_CreateObject();  //创建一个对象 {}
    cJSON *obj2 = cJSON_CreateObject();  //创建一个对象 {}
    cJSON_AddItemToArray(name, obj1);
    cJSON_AddItemToArray(name, obj2);
    printf("%s\n",cJSON_Print(root));

    cJSON_DeleteItemFromArray(name,0);
    printf("%s\n",cJSON_Print(root));

    cJSON_DeleteItemFromObject(root,"name");
    printf("%s\n",cJSON_Print(root));

	return 0;
}

