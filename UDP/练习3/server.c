#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>    
#include <arpa/inet.h>
#include <time.h>
#include <pthread.h>

#define MULTICAST_GROUP "235.255.255.255"
#define PORT 6666

void *receive_thread(void *arg)
{
    int socketfd=*(int *)arg;
    char buf[1024]={0};
    struct sockaddr_in source_address;
    socklen_t addr_len=sizeof(source_address);

    while(1)
    {
        ssize_t len=recvfrom(socketfd,buf,sizeof(buf),0,(struct sockaddr *)&source_address,&addr_len);
        if(len < 0)
        {
            fprintf(stderr, "recvform error,errno:%d,%s\n",errno,strerror(errno));
            continue;
        }
        char *source_ip=inet_ntoa(source_address.sin_addr);//转成点分十进制格式

        time_t current_time;
        struct tm *time_info;
        char formatted_time[20];

        time(&current_time);
        time_info=localtime(&current_time);
        strftime(formatted_time,sizeof(formatted_time),"%Y-%m-%d %H:%M:%S",time_info);
        printf("[%s]\n[%s]:%s\n",formatted_time,source_ip,buf);
    }
}
void *send_thread(void *arg)
{
    int socketfd=*(int *)arg;
    char buf[1024]={0};
    struct sockaddr_in dest_addr;
    socklen_t add_len=sizeof(dest_addr);

    memset(&dest_addr,0,sizeof(dest_addr));
    dest_addr.sin_family=AF_INET;
    dest_addr.sin_port=htons(PORT);
    dest_addr.sin_addr=inet_addr(MULTICAST_GROUP);

    while(1)
    {
        printf("please input:...\n");
        fgets(buf,sizeof(buf),stdin);
        sendto(socketfd,buf,strlen(buf),0,(struct sockaddr *)&dest_addr,&add_len);
    }
}
int main(int argc,char *argv[])
{
    //1.创建UDP套接字             
	int udp_socket = socket(AF_INET, SOCK_DGRAM, 0);
	if (udp_socket == -1)
	{
		fprintf(stderr, "udp socket error,errno:%d,%s\n",errno,strerror(errno));
		exit(1);
	}
    //2.设置 socket 地址重用（多客户端绑定同一端口时需要）(多进程使用同一端口)
    int reuse = 1;
    if (setsockopt(udp_socket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
        fprintf(stderr, "setsocketopt SO_REUSEADDR error,errno:%d,%s\n",errno,strerror(errno));
		exit(2);
    }
    //3.绑定本地地址
    struct sockaddr_in  self_addr;
    memset(&self_addr,0,sizeof(self_addr));
	self_addr.sin_family 		= AF_INET;				
	self_addr.sin_port   		= htons(PORT);
	self_addr.sin_addr.s_addr   = INADDR_ANY;
    if(bind(udp_socket,(struct sockaddr *)&self_addr,sizeof(self_addr)) < 0){
        fprintf(stderr, "bind error,errno:%d,%s\n",errno,strerror(errno));
		exit(3);
    }
    //4.加入多播组
    struct ip_mreq mreq;
    mreq.imr_multiaddr.s_addr = inet_addr(MULTICAST_GROUP);  // 多播组地址
    mreq.imr_interface.s_addr = htonl(INADDR_ANY);           // 监听所有本地接口
    if (setsockopt(udp_socket, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0) {
        fprintf(stderr, "setsockopt IP_ADD_MEMBERSHIP error,errno:%d,%s\n",errno,strerror(errno));
		exit(4);
    }

    printf("等待数据...\n");

    //5.多线程收发
    pthread_t receive_tid,send_tid;
    pthread_create(&receive_tid,NULL,receive_thread,(void *)&udp_socket);
    pthread_create(&send_tid,NULL,send_thread,(void *)&udp_socket);
    //6.等待线程结束
    pthread_join(receive_thread,NULL);
    pthread_join(send_thread,NULL);
    //7.关闭
    close(udp_socket);
    return 0;
}