//
//
//  Socket Class
//  ------------
//  Aceasta clasa este folosita pentru a crea un socket de tip TCP/IP.
//  Sunt 3 tipuri de socketuri pe care clasa o poate crea
//

#pragma once
#include <iostream>
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

class Socket
{
public:
    // Socket simplu care nu primeste niciun parametru
    // Folosit pentru a crea un socket de tip client/acceptare
    Socket();

    // Socket care primeste un parametru
    // Folosit pentru a crea un socket de tip server
    Socket(int PORT);

    // Socket care primeste 2 parametri
    // Folosit pentru a crea un socket de tip client sau connectare
    Socket(int PORT, std::string IP);

    // Destructorul clasei
    ~Socket();

private:
    // File descriptor
    int sockFD;

    // Detalii despre socket
    struct sockaddr_in socket_details;

    // Lungimea structurii de socket
    int socket_details_len;
};
