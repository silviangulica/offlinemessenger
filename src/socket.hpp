#pragma once
#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <fcntl.h>

class Socket
{
public:
    // -- Foloseste pentru a crea un socket de tip ascultare
    Socket(int PORT);

    // -- Foloseste pentru a crea un socket de tip conectare
    Socket(int PORT, char IP[16]);

    // -- Functie pentru returnarea descriptorului socketului
    socklen_t getFD();

    // -- Deconstructor
    ~Socket();

private:
    socklen_t FD;
    struct sockaddr_in socket_details;
};