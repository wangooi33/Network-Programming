

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
#include <netdb.h>

int main(int argc, char const *argv[])
{
	int cnt = 0;
	struct hostent *p = gethostbyname(argv[1]);

	while(  ( p->h_aliases  )[cnt] != NULL  )
	{
		printf("%s\n",   (p->h_aliases)[cnt]   );
		cnt++;
	}


	//输出该域名所有的IP地址
	cnt = 0;


	while(  ( (struct in_addr**)p->h_addr_list  )[cnt] != NULL  )
	{
		printf("%s\n",inet_ntoa(       *(struct in_addr*)(p->h_addr_list)[cnt]       )    );
		cnt++;
	}

	return 0;
}