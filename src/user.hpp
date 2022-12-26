#pragma once
#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>

class User
{
public:
    // -- Constructor pentru clasa User, face automat acceptarea unui client
    User(socklen_t server_socket);

    // -- Seteaza username-ul user-ului
    void setUsername(std::string username);

    // -- Returneaza ID-ul user-ului
    std::string getUsername();

    // -- Transmite un mesaj catre user
    void sendMessage(std::string message);

    // -- Returneaza mesajul primit de la user
    std::string getMessage();

    // -- Returneaza ID-ul user-ului
    socklen_t getID();

    // -- Returneaza parola userului
    std::string getPassword();

    // -- Seteaza parola userului
    void setPassword(std::string password);

    // -- Returneaza daca user-ul este logat sau nu
    bool isLogged();

    // -- Seteaza daca user-ul este logat sau nu
    void setLogged(bool logged);

private:
    socklen_t ID;
    std::string username;
    std::string password;
    bool logged;

    sockaddr_in user_details;
    int user_details_length;
};