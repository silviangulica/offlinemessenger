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
#include <unistd.h>         // Biblioteca standart C

#define MSG_SIZE 32768      // Dimensiunea maxima a unui mesaj, 32KB, sustine aproximativ 300 de utilizatori, probabil mai multi

class Socket
{
public:
    // Socket simplu care nu primeste niciun parametru
    // Folosit pentru a crea un socket de tip client/acceptare
    Socket(socklen_t serverFD);

    // Socket care primeste un parametru
    // Folosit pentru a crea un socket de tip server
    Socket(int PORT);

    // Socket care primeste 2 parametri
    // Folosit pentru a crea un socket de tip client sau connectare
    Socket(int PORT, std::string IP);

    // Functie pentru a returna FD
    socklen_t getFD();

    // Functie pentru a inchide conexiunea
    void closeConnection();

    // Functie pentru a trimite pe socket-ul intern
    void sendMessage(std::string message);

    // Functie pentru a trimite pe un socket extern
    void sendMessage(socklen_t externFD, std::string message);

    // Functie pentru a receptiona pe socket-ul intern
    std::string receiveMessage();

    // Functie pentru a receptiona pe socket-ul extern
    std::string receiveMessage(socklen_t externFD);

    // Destructorul clasei
    ~Socket();

private:
    // File descriptor
    socklen_t sockFD;

    // Detalii despre socket
    struct sockaddr_in socket_details;

    // Lungimea structurii de socket
    int socket_details_len;
};
