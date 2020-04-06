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
#include "server.h"
#define MAX_CONNECTIONS 5

//Global variables
int server_sockfd = 0, client_sockfd = 0, bdFail = 0, lisFail = 0;
ClientList *root, *current;

ClientList *newNode(int sockfd, char* ip) {
    ClientList *np = (ClientList *)malloc(sizeof(ClientList));
    np->data = sockfd;
    np->prev = NULL;
    np->link = NULL;
    np->loggedin = 0;
	np->inchat = 0;
    strncpy(np->ip, ip, 16);
    strncpy(np->name, "NULL", 5);
    return np;
}

char** tokenize(char* buffer){
    char *temp;
    char **data;
    data = (char **)malloc(9* sizeof(char *));
    int i = 0;
    temp = strtok(buffer, ".");
    while (temp != NULL){
        data[i] = temp;
        temp = strtok(NULL, ".");
        i++;
    }
    return data;
}

void displayNumLogged(ClientList *client){
    ClientList *currentUser;
    int count = 0;
    char buffer[2];
    currentUser = root;

    while (currentUser != NULL)
    {
        if(currentUser->loggedin == 1){
            count++;
        }
        currentUser = currentUser->link;
    }
    sprintf(buffer,"%d",count);
    write(client->data, buffer, sizeof(buffer));
}

void groupChat(ClientList *client){
	client->inchat = 1;
	char buffer[30];
	char sender[50];
    ClientList * temp;
	
	while(1)
	{
    temp = root->link;
		
		if(client->inchat == 0)
		{
			break;
		}
		int recieve = recv(client->data, buffer, sizeof(buffer), 0);
		if(recieve > 0)
		{
			sprintf(sender, "%s: %s", client->name, buffer);
		}
		if(strcmp(buffer, "exit\n") == 0)
		{
			printf("user : %s has left global chat\n", client->name);
			sprintf(sender,"%s leave the chatroom", client->name);
			client->inchat = 0;
            while(temp != NULL)
		{
			if(client->data != temp->data&& temp->inchat != 0 )
			{
				printf("Send to sockfd %d: \"%s\"\n", temp->data, sender);
				send(temp->data, sender, sizeof(sender), 0);
			}
			temp = temp->link;
		}
			send(client->data, "0", sizeof("0"), 0);
			return;
		}

		while(temp != NULL)
		{
			if(client->data != temp->data&& temp->inchat != 0 )
			{
				printf("Send to sockfd %d: \"%s\"", temp->data, sender);
				send(temp->data, sender, sizeof(sender), 0);
			}
			temp = temp->link;
		}
	}
	
}

void privateChat(ClientList *client){
    ClientList *temp = root;
    int tog = 0;
    char currentUsers[100] = "";
    char selection[30];
    while (temp != NULL){
        if(temp->loggedin == 1){
            strcat(currentUsers, temp->name);
            strcat(currentUsers, "\n");
        }
        temp = temp->link;
    }
    write(client->data, currentUsers, sizeof(currentUsers));
    recv(client->data, selection, sizeof(selection), 0);
    printf("%s\n", selection);
    temp = root;
    while (temp != NULL){
        if(strcmp(selection, temp->name) == 0){
            tog = 1;
            printf("Usename was found\n");
            break;
        }
        temp = temp->link;
    }
    if(tog != 1){
        printf("Username not found\n");
        write(client->data, "fail", sizeof("fail"));
        return;
    }
    write(client->data, temp->data, sizeof(temp->data));
    
}
void getChatHist(ClientList *client){}
void fileTransfer(ClientList *client){}

void pwordSet(ClientList *client){
    FILE* fp;
    if((fp = fopen("./users.txt", "r+")) == NULL){
        printf("ERROR: File did not open to find username");
        return;
    }
    char buffer[30], newPass[30];
    strcat(client->name, "\n");
    while (fgets(buffer, 30, fp) != NULL){
        if(strcmp(buffer, client->name) == 0){
            fgets(buffer, 30, fp);
            write(client->data, buffer, sizeof(buffer));
            fclose(fp);
            printf("\nUsername has been found\n");
            break;
        }
    }
    recv(client->data, newPass, sizeof(newPass), 0);
    if(strcmp(newPass, "exit") == 0){
        return;
    }
    strcat(newPass, "\n");
    if((fp = fopen("./users.txt", "r+")) == NULL){
        printf("ERROR: File did not open to find password");
        return;
    }
    strcpy(buffer, "\0");
    while (fgets(buffer, 30, fp) != NULL){
        if(strcmp(buffer, client->name) == 0){
            fputs(newPass, fp);
            fclose(fp);
            printf("\nPassword has been changed\n");
            break;
        }
    }

}
void admin_verif(ClientList *client){}

void menu_handler(ClientList *client){
	int life = 1;
	char inp[2];

    while(life){
    recv(client->data, inp, sizeof(inp),0);
		switch (atoi(inp)){
			case 1:
				displayNumLogged(client);
				break;
			case 2:
				groupChat(client);
				break;
			case 3:
				privateChat(client);
				break;
			case 4:
				getChatHist(client);
				break;
			case 5:
				fileTransfer(client);
				break;
			case 6:
				pwordSet(client);
				break;
			case 7:
				client->loggedin = 0;
                return;
				break;
			case 8:
				admin_verif(client);
				break;
			case 0:
				life = 0;
				break;
			}
    }
}
void logUsers(char ***userData, ClientList *client){
    FILE* fp = fopen("users.txt", "a");
    char **uData = *userData;
    fprintf(fp, "%s\n%s\n0\n", uData[1], uData[2]);
    write(client->data, "User has been registered", 25);
    fclose(fp);
}

void logIn(char ***userData, ClientList *client){
    char **uData = *userData;
    char user[30], pass[30];
    int loggedIn = 0;
    char buffer[30];

    strcpy(user,uData[1]);
	strcat(user, "\n");
    strcpy(pass,uData[2]);
	strcat(pass, "\n"); 
    
    FILE* fp;
	
    if((fp = fopen("./users.txt", "r+")) == NULL){
        printf("ERROR: File did not open in login");
        return;
    }
    while ( fgets(buffer, 30, fp) != NULL){
		if(strcmp(user, buffer) == 0){
            
            fgets(buffer, 30, fp);
            if(strcmp(pass, buffer) == 0){
                client->loggedin = 1;
                strcpy(client->name, uData[1]);
                printf("Client %s has logged in\n",client->name);
                write(client->data, "1", 1);
                loggedIn = 1;
                break;
            }
        }
    }
    if(loggedIn == 1){
        fclose(fp);
        menu_handler(client);
    }
    else{
        fclose(fp);
        write(client->data, "0", 1);
    }
}

void login_handler(void *p_client){
    char buffer[90] = "";
    int life = 1;
    ClientList *client;
    char **userData;
    client = (ClientList *) p_client;
    while(life){
        recv(client->data, buffer, sizeof(buffer),0);
        userData = tokenize(buffer);

        switch (atoi(userData[0]))
        {
        case 1:
            logUsers(&userData, client);
            break;
        case 2:
            logIn(&userData, client);
            break;
        default:
            life = 0;
            printf("Client has left the server.\n");
            break;
        }
    }

    close(client->data);
    if (client == current) {
        current = client->prev;
        current->link = NULL;
    } 
    else {
        client->prev->link = client->link;
        client->link->prev = client->prev;
    }
    free(client);
}

int main() {
    server_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(server_sockfd == -1){
        printf("Failed to create socket");
        exit(-1);
    }

    struct sockaddr_in server_info, client_info;
    int s_addrlen = sizeof(server_info);
    int c_addrlen = sizeof(client_info);
    memset(&server_info, 0, s_addrlen);
    memset(&client_info, 0, c_addrlen);
    server_info.sin_family = PF_INET;
    server_info.sin_addr.s_addr = INADDR_ANY;
    server_info.sin_port = htons(60056);

    bdFail = bind(server_sockfd, (struct sockaddr *)&server_info, s_addrlen);
    if(bdFail < 0){
        printf("Failed to bind socket");
        exit(-1);
    }
    
    lisFail = listen(server_sockfd, MAX_CONNECTIONS);
    if(lisFail < 0){
        printf("Failed to listen for connections");
        exit(-1);
    }

    getsockname(server_sockfd, (struct sockaddr*) &server_info, (struct socklen_t*) &s_addrlen);
    printf("Server started on: %s:%d\n", inet_ntoa(server_info.sin_addr), ntohs(server_info.sin_port));

    root = newNode(server_sockfd, inet_ntoa(server_info.sin_addr));
    root->loggedin = 0;
    strcpy(root->name, "Server");
    current = root;

    while(1){
        client_sockfd = accept(server_sockfd, (struct sockaddr*) &client_info, (socklen_t*) &c_addrlen);

        getpeername(client_sockfd, (struct sockaddr*) &client_info, (socklen_t*) &c_addrlen);
        printf("Client %s:%d has joined.\n", inet_ntoa(client_info.sin_addr), ntohs(client_info.sin_port));
        
        ClientList *node;
        node = newNode(client_sockfd, inet_ntoa(client_info.sin_addr));
        node->prev = current;
        current->link = node;
        current = node;

        pthread_t id;
        
        if(pthread_create(&id, NULL, (void*) login_handler, (void*) node) != 0){
            perror("Create pthread error!\n");
            exit(-1);
        }
    }


    
    return 0;
}
