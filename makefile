#Jacob Sansone
#Alexander King
#COP4635
#Project 2

all: Server Client

Server: server.c
	gcc -g -Wall -o Server server.c

Client: client.c
	gcc -g -Wall -o Client client.c

clean:
	rm server client
