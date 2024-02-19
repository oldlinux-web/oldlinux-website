/* Name:	homeipc.c
   Desc:	a client program coupled with the server program homeipd.c
   location:	/usr/bin/
   conf file:	/etc/homeipc.conf
   Version:	0.1
   Jiong.zhao@tongji.edu.cn
   2017.11.13
*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <time.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>

/* 1. Read in the cananical string line in the config file /etc/homeipc.conf file.
      If the config file does not exists, the following default string will be used:
      "My_home_ip_please_00". 
   2. Build the connection socket, and send the string with port 55535 by TCP or UDP.
*/

// Get Home IP -- GHI_
#define GHI_SRV_ADDR	"192.168.1.18"             //oldlinux.org: "139.162.99.40"
#define GHI_PORT	55535       
#define GHI_STR		"My_home_ip_please_00"

int main(int argc, char **argv)
{
        int sockfd, n;
        char    buff[4097];
        struct  sockaddr_in servaddr;


        if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
                exit(1);
        bzero(&servaddr, sizeof(servaddr));
        servaddr.sin_family = AF_INET;
        servaddr.sin_port = htons(GHI_PORT);
        if (inet_pton(AF_INET, GHI_SRV_ADDR, &servaddr.sin_addr) <=0)
                exit(1);

        if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr))<0)
                exit(1);

        snprintf(buff, sizeof(buff), "%s", GHI_STR);
	printf("%s\r\n", buff);
        write(sockfd, buff, strlen(buff));
        close(sockfd);

        exit(0);
}

