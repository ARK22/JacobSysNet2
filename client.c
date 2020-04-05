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

volatile  sig_atomic_t flag = 0;
int sockfd = 0;

void viewNum(int client_socket)
{}
void groupChat(int client_socket)
{
	
}
void privateChat(int client_socket)
{}
void history(int client_socket)
{}
void fileHandler(int client_socket)
{}
void pwordReset(int client_socket)
{}
void admin(int client_socket)
{}
void logout(int client_socket)
{}
void regi(int client_socket)
{
	char user[30], pass[30], toSend[90] = "1.";
	printf("Enter a Username:");
	scanf("%s", user);
	printf("Enter a Password:");
	scanf("%s", pass);
	strcat(toSend, user);
	strcat(toSend, ".");
	strcat(toSend, pass);
	strcat(toSend, "\0");
	write(client_socket, toSend, sizeof(toSend));
	recv(client_socket, toSend, sizeof(toSend), 0);
	printf("\n%s\n", toSend);
}

void userMenu(int client_socket)
{
	int inp, life = 1;
	
	while (life)
	{
		printf("\n-=|  MAIN MENU  |=- \n");
		printf("1. View current online number \n");
		printf("2. Enter the group chat \n");
		printf("3. Enter the private chat  \n");
		printf("4. View chat history \n");
		printf("5. File transfer  \n");
		printf("6. Change the password \n");
		printf("7. Logout \n");
		printf("8. Administrator \n");
		printf("0. Return to the login screen\n");
		printf("Enter an action:");
		scanf("%d", &inp);
		switch (inp)
		{
		case 1:
			viewNum(client_socket);
			break;
		case 2:
			groupChat(client_socket);
			break;
		case 3:
			privateChat(client_socket);
			break;
		case 4:
			history(client_socket);
			break;
		case 5:
			fileHandler(client_socket);
			break;
		case 6:
			pwordReset(client_socket);
			break;
		case 7:
			logout(client_socket);
			break;
		case 8:
			admin(client_socket);
			break;
		case 0:
			life = 0;
			break;
		
		default:
			printf("Invalid Entry, please pick a number between 0 and 2");
			break;
		}
	}
}

void login(int client_socket)
{
 
	char user[30], pass[30], toSend[90] = "2.", isUser[10];
	printf("Enter a Username:");
	scanf("%s", user);
	printf("Enter a Password:");
	scanf("%s", pass);
	strcat(toSend, user);
	strcat(toSend, ".");
	strcat(toSend, pass);
	strcat(toSend, "\0");
	printf("%s", toSend);
	write(client_socket, toSend, sizeof(toSend));
	recv(client_socket, isUser, sizeof(isUser), 0);
	if(atoi(isUser) == 1)
	{
	 userMenu(client_socket);	
	}
	else
	{
		printf("Woops... USername or PasSwoRd is InvaLid");
		return;
	}
 
}

void chatMenu(int client_socket)
{
	
}




void catch_ctrl_c_and_exit(int sig)
{
 flag = 1;	
}

void error(char *msg)
{
    perror(msg);
    exit(-1);
}

void recv_msg_handler()
{
	
}

void send_msg_handler()
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
   server_info.sin_addr.s_addr = INADDR_ANY;
   server_info.sin_port = htons(60056);
   
   int err = connect(sockfd,(struct sockaddr *)&server_info, s_addrlen);
   if(err == -1)
   {
	   printf("Connection to Server error! \n");
	   exit(EXIT_FAILURE);
   }
   
   getsockname(sockfd,(struct sockaddr*) &client_info, (socklen_t *) &c_addrlen);
   getpeername(sockfd,(struct sockaddr*) &server_info, (socklen_t *) &s_addrlen);
   printf("Connect to Server: %s:%d\n", inet_ntoa(server_info.sin_addr), ntohs(server_info.sin_port));
   
   /*send(sockfd, "hello", LENGTH_NAME, 0);
   
	pthread_t recv_msg_thread;
	recv_msg_thread = pthread_create(&recv_msg_thread,NULL,(void *) recv_msg_handler, NULL);
	if(recv_msg_thread != 0)
	{
		printf("Create pthread error!\n");
		exit(EXIT_FAILURE);
	}
	
	*/
	int inp, life = 1;
	while(life)
	{
		printf("\n-=| Online Chat Room |=-\n");
		printf("1. Register\n");
		printf("2. Login\n");
		printf("0. QUIT\n");
		printf("Enter an action: ");
		scanf("%d", &inp);
		switch (inp)
		{
		case 1:
			regi(sockfd);
			break;
		case 2:
			login(sockfd);
			break;
		case 0:
			life = 0;
			write(sockfd, "0", 1);
			break;
		
		default:
			printf("Invalid Entry, please pick a number between 0 and 2");
			break;
		}

	}
	
	close(sockfd);
    return 0;
}
