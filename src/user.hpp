#pragma once
#include <string>
#include <iostream>
#include "socket.hpp"
class User
{
public:
    // Constructor
    User(std::string username, std::string password, Socket* socket);

    // Getters and setters
    std::string getUsername();
    std::string getPassword();
    std::string getUserColor();
    std::string getPersonalStatus();
    bool isLoggedIn();
    Socket* getSocket();

    void setUsername(std::string username);
    void setPassword(std::string password);
    void setSocket(Socket* socket);
    void setLoggedIn(bool isLogged);
    void setUserColor(std::string color);
    void setPersonalStatus(std::string status);


private:
    std::string username;
    std::string password;
    std::string userColor;
    std::string personalStatus; // UTF-8
    bool isLogged;
    Socket* socket;
};