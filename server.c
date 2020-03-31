/*
* Jacob Sansone
* Alexander King
* COP4635
* Project 1
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>

/*Used for error detection during all steps of creating and connecting the sockets.*/
void error(char *msg)
{
    perror(msg);
    exit(-1);
}

/*This is responsible for parsing the get request when recieved
 and pulling the name of the file out.*/
char* parser(char *tokenize)
{ 
    char* startOfFile = strchr(tokenize, '/');
    char *endOfFile = strchr(startOfFile, ' ');
    char *buffer;

    buffer = malloc(sizeof(char) *(endOfFile - startOfFile));

    strncpy(buffer, startOfFile, endOfFile-startOfFile);

    buffer[strlen(buffer)] = 0;

    return buffer;
}

int main() 
{
    /*Creation and initialization of variables needed*/
    char buffer[256], buff2[256];
    char *filename;
    struct sockaddr_in tcp_server_address;
    int tcp_server_socket, tcp_client_socket , n, l, byte_size;
    FILE *fp;

    
    
    /*Creating the server socket*/
    tcp_server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if(tcp_server_socket < 0)
    {
        error("Error: Socket Creation Failure\n");
    }
    printf("Socket Created Successfully\n");

    
    
    /*Defining the paramters of the server socket: 
       -------------------> The port to listen on
       -------------------> The TCP/IP protocol it will follow
       -------------------> Setting the IP to localhost*/
    tcp_server_address.sin_family = AF_INET;
    tcp_server_address.sin_port = htons(60056);
    tcp_server_address.sin_addr.s_addr = INADDR_ANY;





    /*Binding the socket to the parameters we just set.*/
    n = bind(tcp_server_socket, (struct sockaddr *) &tcp_server_address, sizeof(tcp_server_address));
    if(n < 0)
    {
        error("Error: Socket Failed To Bind\n");
    }
    printf("Socket Successfully Bound To Port\n");
    




    /*Telling the socket to listen on the port for requests and setting an allowed number to be in the queue*/
    l = listen(tcp_server_socket, 5);
    if(l < 0)
    {
        error("Error: Socket Failed To Listen\n");
    }
    printf("Socket Is Listening For Connections\n");
    






    /* A while loop that was made to continuously
     wait for connection from either the client or the browser and handle the requests as given.*/
    while(1)
    {
        tcp_client_socket = accept(tcp_server_socket, NULL, NULL);
        printf("Client has connected\n");
        byte_size = recv(tcp_client_socket, buffer, sizeof(buffer)-1, 0);
        buffer[byte_size] = '\0';
        filename = parser(buffer);
        memset(buffer, 0, strlen(buffer));
        /*If the file after parsing is the file we need.
         Open and read through it sending the desired document back to the client if found,
          otherwise error.*/
        if(strcmp(filename, "/index.html") == 0)
        {
            fp = fopen("./index.html", "r");
            if(fp == NULL){
                error("Failed to read file");
            }
            else{
                strncpy(buffer, "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n", 256);
                while (fgets(buff2, 256, fp) != NULL)
                {
                    strncat(buffer, buff2, 256);
                }
                fclose(fp);
                buffer[sizeof(buffer)] = '\0';
                write(tcp_client_socket, buffer, 256);
                memset(buffer, 0, 256);
            }
        }
        else if(strcmp(filename, "/index2.html") == 0)
        {
            fp = fopen("./index2.html", "r");
            if(fp == NULL){
                error("Failed to read file");
            }
            else{
                strncpy(buffer, "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n", 256);
                while (fgets(buff2, 256, fp) != NULL)
                {
                    strncat(buffer, buff2, 256);
                }
                fclose(fp);
                buffer[sizeof(buffer)] = '\0';
                write(tcp_client_socket, buffer, 256);
                memset(buffer, 0, 256);
            }
        }
        else{
            write(tcp_client_socket, "HTTP/1.1 404 NOT FOUND\r\n\r\n", 256);
        }
        /*And close the socket, free the fileName and clear the buffer when complete.*/
        close(tcp_client_socket);
        printf("Client has disconnected\n");
        memset(filename, 0, strlen(filename));
        free(filename);
    }
    return 0;
}
