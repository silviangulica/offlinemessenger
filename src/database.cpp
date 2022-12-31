#include "database.hpp"

Database::Database(std::string filename)
{
    this->filename = filename;
}

User* Database::findUser(std::string username)
{
    std::string actuall_address = "./src/" + this->filename + "/" + username;
    std::ifstream file(actuall_address);
    User *user = new User("", "", NULL);
    std::string line;

    if (!file.is_open()) 
       return NULL;

    // Citim fiecare linie din fisier
    file >> line;
    user->setUsername(line);
    file >> line;
    user->setPassword(line);

    // Inchidem si returnam userul
    file.close();
    return user;
}

void Database::addUser(User user)
{
    users.push_back(user);
}

void Database::removeUser(std::string username)
{
    for (int i = 0; i < users.size(); i++)
    {
        if (users[i].getUsername() == username)
        {
            users.erase(users.begin() + i);
            return;
        }
    }
}