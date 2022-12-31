#include "user.hpp"

User::User(std::string username, std::string password, Socket *socket)
{
    this->username = username;
    this->password = password;
    this->socket = socket;
}

std::string User::getUsername()
{
    return this->username;
}

std::string User::getPassword()
{
    return this->password;
}

void User::setUsername(std::string username)
{
    this->username = username;
}

void User::setPassword(std::string password)
{
    this->password = password;
}

void User::setSocket(Socket *socket)
{
    this->socket = socket;
}
