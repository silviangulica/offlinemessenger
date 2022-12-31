all: socket.o user.o database.o
	g++ -Wall -o bin/server src/server.cpp socket.o user.o database.o
	g++ -Wall -o bin/client src/client.cpp socket.o
	rm -f *.o

socket.o: src/socket.cpp src/socket.hpp
	g++ -Wall -c src/socket.cpp

user.o: src/user.cpp src/user.hpp
	g++ -Wall -c src/user.cpp

database.o: src/database.cpp src/database.hpp
	g++ -Wall -c src/database.cpp



clean:
	rm -f server client