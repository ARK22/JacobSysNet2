/*
* Jacob Sansone
* Alexander King
* COP4635
* Project 2
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "string.h"

volatile sig_automatic_t flag = 0;

int sockfd = 0;

void catch_ctrl_c_and_exit(int sig)
{
 flag = 1;	
}

void error(char *msg)
{
    perror(msg);
    exit(-1);
}

void recv()
{
	
}

void send()
{
	
}

int main()
{
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd == -1)
	{
		printf("Fail to create a socket");
		exit(EXIT_FAILURE);
	}
   
   struct sockaddr_in server_info, client_info;
   int s_addrlen = sizeof(server_info);
   int c_addrlen = sizeof(client_info);
   memset(&server_info, 0, s_addrlen);
   memset(&client_info, 0, c_addrlen);
   /*port num = 60056*/
   server_info.sin_family = PF_INET;
   server_info.sin_addr.s_addr = inet_addr(INADDR_ANY);
   server_info.sin_port = htons(60056);
   
   int err = connect(sockfd,(struct sockaddr *)&server_info, s_addrlen);
   if(err == -1)
   {
	   printf("Connection to Server error! \n");
	   exit(EXIT_FAILURE);
   }
   
   getsockname(sockfd,(struct sockaddr*) &client_info, (socklen_t *) &c_addrlen);
   getpeername(sockfd,(struct sockaddr*) &server_info, (socklen_t *) &s_addrlen);
   printf("Connect to Server: %s:%d\n", inet_ntoa(server_info.sin_addr), ntohs(client_info.sin_port));
   
   /*send(sockfd, "hello", LENGTH_NAME, 0);*/
   
   pthread_t recv_msg_thread;

	if((pthread_create(&recv_msg_thread,NULL,(void *) recv_msg_handler, NULL)!= 0)
	{
		printf("Create pthread error!\n");
		exit(EXIT_FAILURE);
	}
	
	while(1)
	{
		if(flag)
		{
			printf("\nbye\n");
			break;
		}
	}
	
	close(sockfd);
    return 0;
}
