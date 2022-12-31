#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include "user.hpp"

class Database
{
public:
    // Constructor
    Database(std::string filename);

    // Methods
    void addUser(User user);
    void removeUser(std::string username);
    User* findUser(std::string username);

private:
    std::string filename;
    std::vector<User> users;
};