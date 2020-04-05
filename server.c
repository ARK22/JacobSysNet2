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

void logUsers(char ***userData, ClientList *client){
    FILE* fp = fopen("users.txt", "a");
    char **uData = *userData;
    fprintf(fp, "%s\n%s\n0\n0\n", uData[1], uData[2]);
    write(client->data, "User has been registered", 25);
    fclose(fp);
}

void logIn(char ***userData, ClientList *client){
    char **uData = *userData;
    char *user, *pass;
    char test;
    int loggedIn = 0;
    char buffer[30];

    user = uData[1];
    pass = uData[2];
    printf("%s\n", user);
    printf("%s\n", pass);
    //strcat(temp, "\n");

    
    FILE* fp = fopen("./users.txt", "r+");
    if(fp == NULL){
        printf("ERROR: File did not open");
        return;
    }
    printf("HERE");
    fgets(buffer, 30, fp);
    printf("HERE2");
    while (1){
        
        if(strcmp(user, buffer) == 0){
            printf("%s\n", pass);
            //strcat(temp, "\n");
            fgets(buffer, 30, fp);
            if(strcmp(pass, buffer) == 0){
                write(client->data, "1", 1);
                fputs("1", fp);
                loggedIn = 1;
                break;
            }
        }
        test = fgetc(fp);
        if(test == EOF){
            break;
        }
        ungetc(test, fp);
    }
    if(loggedIn == 1){
        printf("NOICE\n");
        //menu_handler(client);
    }
    else{
        write(client->data, "0", 1);
    }

    fclose(fp);
    
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
        case 0:
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
