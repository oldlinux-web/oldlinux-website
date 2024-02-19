/* Name:	homeipd.c
   Desc:	a server program coupled with the client program homeipc.c
   location:	/usr/bin/
   conf file:	/etc/homeipd.conf
   output file: /var/www/website/homeip.txt
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
#include <sys/stat.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "iniparser.h"


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
#define GHI_INI_FILE	"/etc/homeip.ini"
#define SEC_SERVER	"server"               // Should be lower chars!!
#define GHI_PORT	"55535"       
#define GHI_OUT_FILE	"/var/www/homepage/oldlinux.org/homeip.txt"
#define SEC_CLIENTS	"clients"              // Should be lower chars!!
#define GHI_STR0	"My_home_ip_please_00"
#define GHI_STR1	"My_home_ip_please_01"

dictionary      *inifd;
int		ghi_port;
char    	*ghi_out_file = GHI_OUT_FILE;
char		**ghi_strs;  
int		ghi_strs_num;

int parse_ini_file(char *ini_name);
void create_ini_file(char *ini_name);

int main(int argc, char **argv)
{
        socklen_t       len;
        int     listenfd, connfd, n;
        char    buff[4096], buff1[4096];
        struct sockaddr_in servaddr, cliaddr;
        time_t  ticks;
	int	outfd;
	int	curr;
	char    *def_str, *cli_str;
	char	**pptr;

	printf("Get Home IP address Server.\nVersion 0.3 \nby Jiong.Zhao@tongji.edu.cn \n------\n");
	if ((parse_ini_file(GHI_INI_FILE)) < 0) 
		exit(1);
	printf("Begin networking...\n");	

        listenfd = socket(AF_INET, SOCK_STREAM, 0);
        bzero(&servaddr, sizeof(servaddr));
        servaddr.sin_family = AF_INET;
        servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
        servaddr.sin_port = htons(ghi_port);

        bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
        listen(listenfd, 1024);

        for (;;) {
                len = sizeof(cliaddr);
                connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &len);
	        if (( n = read(connfd, buff, 4097)) > 0) {   // while
                	buff[n] = 0;
			pptr = ghi_strs;
			for (curr = 0; curr < ghi_strs_num; curr++) {
				def_str = *pptr++;
				cli_str = iniparser_getstring(inifd, def_str, NULL);
        			if ( !strncmp(cli_str, buff, n)) {
/*  //  */					printf("key & val: %s, %s\n", def_str, cli_str);
		        		if ((outfd = open(ghi_out_file, O_WRONLY | O_CREAT | O_APPEND, 
		        				S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)) == -1) {
                				printf("Error open file: %s.\n", ghi_out_file);
                				exit(1);
        				} else {
						inet_ntop(AF_INET, &cliaddr.sin_addr, buff1, sizeof(buff1));
						ticks = time(NULL);
						sprintf(buff, "%.24s, %s, %s\n", ctime(&ticks), def_str, buff1);
						write(outfd, buff, strlen(buff));
						close(outfd);
					}
				}
			}
			close(connfd);	
        	} 
		else close(connfd);
	}
}

int parse_ini_file(char *ini_name)
{
	int	i, n = 0;
	char	*sec_name = SEC_CLIENTS;
	char    *def_str  = GHI_STR0;
	char    **pptr;

	inifd = iniparser_load(ini_name); 
	printf("Begining parse ini_file: %s\n", ini_name);
	if (inifd == NULL) {
		printf( "Ini file not found: %s, auto created. \n", ini_name);
		create_ini_file(ini_name);
		inifd = iniparser_load(ini_name);
		if (inifd == NULL) {
			fprintf(stderr, " Error creating ini file: %s !\n", ini_name);
			return -1;
		}
	}
	ghi_port = iniparser_getint(inifd, "Server:port", -1);
	if (ghi_port < 80) {
		fprintf(stderr, "Server Port not correct: %d\n", ghi_port);
		return -1;
	}
	printf("network port: %d\n", ghi_port);

	ghi_out_file = iniparser_getstring(inifd, "Server:OutputFile", NULL);
	if (ghi_out_file == NULL) {
		fprintf(stderr, "Output file error. \n");
		return -1;
	}
	printf("Output file: %s\n", ghi_out_file);

	n = iniparser_getsecnkeys(inifd, SEC_CLIENTS);
	if (n == 0) { 
		fprintf(stderr, "No user present.\n");
		return -1;
	} else {
		printf("Clients: %d\n", n);
		ghi_strs_num = n;
		ghi_strs = iniparser_getseckeys(inifd, SEC_CLIENTS);
		pptr = ghi_strs;
		for (i=0; i< ghi_strs_num; i++) {
			def_str = *pptr++;
			printf("%s\n", def_str);
		}
	}
	return 0;
}

   
void create_ini_file(char *ini_name)
{
	FILE	*ini;

	printf("Begining create_ini_file ... \n");	
	ini = fopen(ini_name, "w");
	fprintf(ini, 
	"#\n"
	"# The ini file for get home ip program server.\n"
	"#\n"
	"\n"
	"[Server]\n"
	"Port		= " 
	GHI_PORT
	"\n"
	"OutputFile	= "
	GHI_OUT_FILE 
	"\n"
	"\n"
	"\n"
	"[Clients]\n"
	"PassString0	= " 
	GHI_STR0 
	"\n"
	"PassString1	= " 
	GHI_STR1 
	"\n"
	"\n");
	fclose(ini);
}

	
	


