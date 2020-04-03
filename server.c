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

void client_handler(void *p_client){
    char buffer[90] = "";
    ClientList *client = (ClientList *) p_client;
    recv(client->data, buffer, sizeof(buffer),0);
    printf("%s", buffer);

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

int main() 
{
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
    current = root;

    while(1){
        client_sockfd = accept(server_sockfd, (struct sockaddr*) &client_info, (socklen_t*) &c_addrlen);

        getpeername(client_sockfd, (struct sockaddr*) &client_info, (socklen_t*) &c_addrlen);
        printf("Client %s:%d has joined.\n", inet_ntoa(client_info.sin_addr), ntohs(client_info.sin_port));

        ClientList *node = newNode(client_sockfd, inet_ntoa(client_info.sin_addr));
        node->prev = current;
        current->link = node;
        current = node;

        pthread_t id;
        if(pthread_create(&id, NULL, (void*) client_handler, (void*) node) != 0){
            perror("Create pthread error!\n");
            exit(-1);
        }
    }


    
    return 0;
}
