all: socket.o
	g++ -Wall -o server server.cpp socket.o
	g++ -Wall -o client client.cpp socket.o

socket.o: src/socket.cpp src/socket.hpp
	g++ -Wall -c src/socket.cpp