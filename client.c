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
#include <errno.h>
/*Used for error detection during all steps of creating and connecting the sockets.*/
void error(char *msg)
{
    perror(msg);
    exit(-1);
}


int main(int argc, char* argv[]){
   
   
   /*check to ensure we are passing a file*/
    if(argc < 2)
    {
        printf("Usage: %s filename\n", argv[0]);
        exit(-1);
    }
    
    
    
    /*creating required varibles*/
    int tcp_client_socket, connection_status;
    struct sockaddr_in tcp_server_address;
    char buffer[256];
    
    
    
    
    /*Parsing the http header to send.*/
    memset(buffer, 0, sizeof(buffer));
    strcpy(buffer, "GET /");
    strcat(buffer, argv[1]);
    strcat(buffer,  " HTTP/1.1\r\nHost: localhost\r\n\r\n");


    
    
    
    /*Creating the client socket*/
    tcp_client_socket = socket(AF_INET, SOCK_STREAM, 0);
    
    
    
    
    
    /*Defining the paramters of the client socket: 
       -------------------> The port to request from
       -------------------> The TCP/IP protocol it will follow
       -------------------> Setting the IP to localhost*/
    tcp_server_address.sin_family = AF_INET;
    tcp_server_address.sin_port = htons(60056);
    tcp_server_address.sin_addr.s_addr = INADDR_ANY;

    /*attempting to connect to a server on the port it is set to listen on*/
    connection_status = connect(tcp_client_socket, (struct sockaddr *) &tcp_server_address, sizeof(tcp_server_address));
    if (connection_status == -1)
    {
        printf("Socket failed to connect\n");
     }
    
    /*Send request to the server*/
    send(tcp_client_socket, buffer, sizeof(buffer), 0);
    printf("Sent message:\n \"%s\" '\n", buffer);
    
    /*clear buffer*/
    memset(buffer, 0 ,strlen(buffer));
    
    /*Recieve response from server*/
    recv(tcp_client_socket, buffer, sizeof(buffer) - 1, 0);
    
    /*print recieved message*/
    buffer[sizeof(buffer)] = '\0';
    printf("Received Message:\n \"%s\" \n", buffer);

    /*Closing connection after all interactions are completed.*/
    close(tcp_client_socket);

    return 0;
}
