all: client.o server.o socket.o user.o database.o
	g++ -Wall client.o socket.o -o client
	g++ -Wall server.o socket.o user.o database.o -o server
	rm -f *.o

client.o: client.cpp
	g++ -Wall -c client.cpp

server.o: server.cpp
	g++ -Wall -c server.cpp

socket.o: src/socket.cpp src/socket.hpp
	g++ -Wall -c src/socket.cpp

user.o: src/user.cpp src/user.hpp
	g++ -Wall -c src/user.cpp

database.o: src/database.cpp src/database.hpp
	g++ -Wall -c src/database.cpp

clean:
	rm -f *.o client server