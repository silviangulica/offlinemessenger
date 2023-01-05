#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include "user.hpp"
#include "message.hpp"

class Database
{
public:
    // Constructor
    Database(std::string filename);

    // Methods
    User* findUser(std::string username);
    User* getUser(std::string username);
    User* getUserBySocket(Socket* socket);
    std::string getUsers();

    void loadMessagesAndSend(std::string username, Socket* client);
    void updateMessageFile(Message* message, std::string username);
    void removeUser(std::string username);
    void addUser(User* user);
    void createUserFile(User* user);

private:
    std::string filename;
    std::vector<User*> users;
};