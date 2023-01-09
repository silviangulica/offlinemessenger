#include "user.hpp"

User::User(std::string username, std::string password, Socket* socket)
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

void User::setSocket(Socket* socket)
{
    this->socket = socket;
}

bool User::isLoggedIn()
{
    return this->isLogged;
}

void User::setLoggedIn(bool isLogged)
{
    this->isLogged = isLogged;
}

Socket* User::getSocket()
{
    return this->socket;
}

std::string User::getUserColor()
{
    return this->userColor;
}

void User::setUserColor(std::string color)
{
    this->userColor = color;
}

void User::setPersonalStatus(std::string status)
{
    this->personalStatus = status;
}

std::string User::getPersonalStatus()
{
    return this->personalStatus;
}