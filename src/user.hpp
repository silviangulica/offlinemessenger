#pragma once
#include <string>
#include <iostream>
#include "socket.hpp"
class User
{
public:
    // Constructor
    User(std::string username, std::string password, Socket *socket);

    // Getters and setters
    std::string getUsername();
    std::string getPassword();
    bool isLoggedIn();
    Socket* getSocket();

    void setUsername(std::string username);
    void setPassword(std::string password);
    void setSocket(Socket *socket);
    void setLoggedIn(bool isLogged);


private:
    std::string username;
    std::string password;
    bool isLogged;
    Socket *socket;
};