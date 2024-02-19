/* Name:	homeipd.c
   Desc:	a server program coupled with the client program homeipc.c
   location:	/usr/bin/
   conf file:	/etc/homeipd.conf
   output file: /var/www/website/homeip.txt
   Version:	0.2
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

#include <sys/stat.h>
#include <fcntl.h>


/* 1. Read in the canonical string lines in the config file /etc/homeipd.conf file to
      the array homeids[128, 256], and check the output file /var/www/website/homeip.txt.
      If the config file does not exists, 10 default strings will be used:
      "My_home_ip_please_00" to "My_home_ip_please_10".
      The output file /var/www/website/homeip.txt will be created if it does not exist.
   2. Establish network link and listen to the port 55535. If a message string is received,
      it is compared with the strings in array homeids[]. If the received string is actually
      contained in the string array homeids[], then the link's source IP will be obtained 
      and stored in the output file with the string in a line.
*/

// Get Home IP -- GHI_
#define GHI_PORT	55535       
#define GHI_STR		"My_home_ip_please_00"
#define GHI_OUT_FILE	"/var/www/homepage/oldlinux.org/homeip.txt"


int main(int argc, char **argv)
{
        socklen_t       len;
        int     listenfd, connfd, n;
        char    buff[4096], buff1[4096];
        struct sockaddr_in servaddr, cliaddr;
        time_t  ticks;
	int	outfd;

        listenfd = socket(AF_INET, SOCK_STREAM, 0);
        bzero(&servaddr, sizeof(servaddr));
        servaddr.sin_family = AF_INET;
        servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
        servaddr.sin_port = htons(GHI_PORT);

        bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
        listen(listenfd, 1024);

        for (;;) {
                len = sizeof(cliaddr);
                connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &len);
	        if (( n = read(connfd, buff, 4097)) > 0) {   // while
                	buff[n] = 0;
	//		printf("Received String: %s, %d %d\r\n", buff, n, sizeof(GHI_STR));
        		if ( !strncmp(GHI_STR, buff, n)) {
//	               		printf( "%s, %s\n", buff, 
//	       		 		inet_ntop(AF_INET, &cliaddr.sin_addr, buff1, sizeof(buff1)));
		        	if ((outfd = open(GHI_OUT_FILE, O_WRONLY | O_CREAT | O_APPEND, 
							S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)) == -1) {
                			printf("Error open file: %s.\n", GHI_OUT_FILE);
                			exit(1);
        			}
				else {
					inet_ntop(AF_INET, &cliaddr.sin_addr, buff1, sizeof(buff1));
					ticks = time(NULL);
					sprintf(buff, "%s, %.24s, %s\n", buff, ctime(&ticks), buff1);
					write(outfd, buff, strlen(buff));
					close(outfd);
				}
			}
			close(connfd);	
        	} 
		else close(connfd);
	}
}
   
   


