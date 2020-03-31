#Jacob Sansone
#Alexander King
#COP4635
#Project 2

all: Server Client

httpServer: httpServer.c
	gcc -g -Wall -o Server server.c

httpClient: httpClient.c
	gcc -g -Wall -o Client client.c

clean:
	rm server client