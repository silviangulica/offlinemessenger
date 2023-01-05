#include "database.hpp"

Database::Database(std::string filename)
{
    this->filename = filename;
}

User* Database::findUser(std::string username)
{
    std::string actuall_address = this->filename + "/" + username;
    std::ifstream file(actuall_address);
    User* user = new User("", "", NULL);
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

void Database::addUser(User* user)
{
    users.push_back(user);
}

void Database::removeUser(std::string username)
{
    for (int i = 0; i < users.size(); i++)
    {
        if (users[i]->getUsername() == username)
        {
            users.erase(users.begin() + i);
            delete users[i];
            return;
        }
    }
}

User* Database::getUser(std::string username)
{
    for (int i = 0; i < users.size(); i++)
    {
        if (users[i]->getUsername() == username)
        {
            return users[i];
        }
    }

    return NULL;
}

void Database::createUserFile(User* user)
{
    std::string actuall_address = this->filename + "/" + user->getUsername();
    std::ofstream file(actuall_address);

    file << user->getUsername() << std::endl;
    file << user->getPassword() << std::endl;

    file.close();
    return;
}

User* Database::getUserBySocket(Socket* socket)
{
    for (int i = 0; i < users.size(); i++)
    {
        if (users[i]->getSocket() == socket)
        {
            return users[i];
        }
    }

    return NULL;
}


std::string Database::getUsers()
{
    std::string users = "Users:\n";
    for (int i = 0; i < this->users.size(); i++)
    {
        int id = this->users[i]->getSocket()->getFD();
        std::string id_string = std::to_string(id);
        users += "[" + id_string + "] " + this->users[i]->getUsername() + " " + this->users[i]->getPassword() + "\n";
    }

    users.pop_back();
    return users;
}

void Database::updateMessageFile(Message* message, std::string username)
{
    std::string actuall_address = this->filename + "/messages/" + username;
    std::ofstream file(actuall_address, std::ios::app);

    file << message->toString() << std::endl;

    file.close();
    return;
}

void Database::loadMessagesAndSend(std::string username, Socket* client)
{
    std::string actuall_address = this->filename + "/messages/" + username;
    std::ifstream file(actuall_address);
    std::string line;

    if (!file.is_open())
        return;

    // Citim fiecare linie din fisier
    client->sendMessage("Mesajele tale cat timp ai fost offline:\n");
    while (getline(file, line))
    {
        client->sendMessage(line + "\n");
    }

    // Inchidem si returnam userul
    file.close();

    // Stergem fisierul
    if (std::remove(actuall_address.c_str()) == -1)
    {
        std::cout << "Error deleting file of user " << username << std::endl;
    }
    return;
}