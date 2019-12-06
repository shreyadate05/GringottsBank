# Makefile for the socket programming example
#
server_objects = GringottsServer.o Socket.o ServerMain.o 
client_objects = GringottsClient.o Socket.o ClientMain.o 

all : server client

server: $(server_objects)
	g++ -pthread -o server $(server_objects)

client: $(client_objects)
	g++ -pthread -o client $(client_objects)

Socket: Socket.cpp
GringottsServer: GringottsServer.cpp
GringottsClient: GringottsClient.cpp
server_main: ServerMain.cpp
client_main: ClientMain.cpp

clean:
	rm -rf *.o server client
