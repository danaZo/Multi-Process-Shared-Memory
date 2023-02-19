CC = g++
all: server client BonusServer BonusClient

client: client.o
	$(CC) -g client.cpp -o client

server: server.o stack.o 
	$(CC)  stack.o server.o -o server -lpthread

BonusClient: BonusClient.o
	$(CC) -g BonusClient.cpp -o BonusClient	

BonusServer: BonusServer.o stack.o
	$(CC) stack.o BonusServer.o -o BonusServer -lpthread	

stack.o: stack.cpp
	$(CC) -g -c stack.cpp

BonusServer.o: BonusServer.cpp
	$(CC) -g -c BonusServer.cpp

server.o: server.cpp
	$(CC) -g -c server.cpp

clean:
	rm *.o server client BonusServer BonusClient