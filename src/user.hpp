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

    void setUsername(std::string username);
    void setPassword(std::string password);
    void setSocket(Socket *socket);


private:
    std::string username;
    std::string password;
    Socket *socket;
};