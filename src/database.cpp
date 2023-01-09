#include "database.hpp"
#include "message.hpp"
#include <sys/stat.h>
#include <dirent.h>
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
    file >> line;
    std::string color = "\033[1;";
    color += line + "m";
    user->setUserColor(color);

    // Artificiu again, very important, status trebuie sa fie la final altfel nu o sa il citeasca normal
    std::string status;
    while (file >> line)
        status += line + " ";
    user->setPersonalStatus(status);



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

    // Pentru culoare facem un pic de aritificu, intrucat salvam doar ID la culoare
    std::string color = std::string(user->getUserColor(), 4, 2);
    file << color << std::endl;

    // Important din nou! Statusul trebuie sa fie la final altfel nu o sa il citeasca normal
    file << user->getPersonalStatus() << std::endl;

    file.close();

    // Creeam un director pentru mesaje
    actuall_address = this->filename + "/messages/" + user->getUsername();
    mkdir(actuall_address.c_str(), 0777);
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
        std::string user_with_color = this->users[i]->getUserColor() + this->users[i]->getUsername() + "\033[0m";
        users += "[" + id_string + "] " + user_with_color + "\t\"" + this->users[i]->getPersonalStatus() + "\"\n";
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
    std::string actuall_address = this->filename + "/messages/" + username + "/";
    int ID = 0;

    // Cautam rand pe rand fiecare id care poate sa ramana in director, daca nu exista unul, ne oprim
    // ID sunt mereu in ordnie crescatoare , 0, 1, 2, ... , n
    while (true) {
        std::string file_name = actuall_address + std::to_string(ID);
        std::ifstream file(file_name);

        if (!file.is_open())
            break;

        std::string line;
        while (std::getline(file, line))
        {
            client->sendMessage(line);
        }

        file.close();
        ID++;
    }
    return;
}

void Database::updateUserFile(User* user)
{
    // Stergem fisierul existent
    std::string actuall_address = this->filename + "/" + user->getUsername();

    if (std::remove(actuall_address.c_str()) == -1)
    {
        std::cout << "Error deleting file of user " << user->getUsername() << std::endl;
        std::cout << "[Error]: There was an error deleting the file of user " << user->getUsername() << " , I exit because there is a problem with db/code" << std::endl;
        exit(1);
    }

    // Cream un nou fisier
    std::ofstream file(actuall_address);

    // Introducem datale in fisier
    file << user->getUsername() << std::endl;
    file << user->getPassword() << std::endl;

    // Pentru culoare facem un pic de aritificu, intrucat salvam doar ID la culoare
    std::string color = std::string(user->getUserColor(), 4, 2);
    file << color << std::endl;

    // Important din nou! Statusul trebuie sa fie la final altfel nu o sa il citeasca normal
    file << user->getPersonalStatus() << std::endl;

    file.close();
}

void Database::updateUserFile(User* user, std::string oldUserName)
{
    // Stergem fisierul existent
    std::string actuall_address = this->filename + "/" + oldUserName;

    if (std::remove(actuall_address.c_str()) == -1)
    {
        std::cout << "Error deleting file of user " << user->getUsername() << std::endl;
        std::cout << "[Error]: There was an error deleting the file of user " << user->getUsername() << " , I exit because there is a problem with db/code" << std::endl;
        exit(1);
    }

    // Cream un nou fisier
    actuall_address = this->filename + "/" + user->getUsername();
    std::ofstream file(actuall_address);

    // Introducem datale in fisier
    file << user->getUsername() << std::endl;
    file << user->getPassword() << std::endl;

    // Pentru culoare facem un pic de aritificu, intrucat salvam doar ID la culoare
    std::string color = std::string(user->getUserColor(), 4, 2);
    file << color << std::endl;

    file.close();
}

int Database::getNumberOfMessages(std::string username)
{
    // Citim din director
    DIR* dir;
    struct dirent* ent;
    int number_of_messages = 0;

    dir = opendir((this->filename + "/messages/" + username).c_str());

    if (dir != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            // Verificam daca este fisier
            if (ent->d_type == DT_REG) {
                number_of_messages++;
            }
        }
        closedir(dir);
    }
    else {
        std::cout << "Error opening directory" << std::endl;
    }

    return number_of_messages;
}

void Database::addMessageFile(Message* msg, std::string username)
{
    std::string actuall_address = this->filename + "/messages/" + username + "/" + std::to_string(msg->getID());
    std::ofstream file(actuall_address, std::ios::app);

    file << msg->toString() << std::endl;

    file.close();
    return;
}

void Database::deleteAllMessages(std::string username)
{
    std::string actuall_address = this->filename + "/messages/" + username + "/*";
    int status = system(("rm -rf " + actuall_address).c_str());
    if (status == -1)
    {
        std::cout << "Error deleting messages of user : " << username << std::endl;
        return;
    }

    // Cream directorul
    actuall_address = this->filename + "/messages/" + username;
    mkdir(actuall_address.c_str(), 0777);
}

std::string Database::findMessage(std::string messageID, std::string username) {
    std::string actuall_address = this->filename + "/messages/" + username + "/" + messageID;
    std::ifstream file(actuall_address);

    if (!file.is_open())
        return "";

    std::string line;
    std::string message = "";
    std::getline(file, line);
    message = std::string(line, 4, line.size() - 4);
    return message;
}

void Database::sendMessageToAll(std::string username, std::string message) {
    // Cautam utilizatorul care are username-ul dat
    User* user = this->getUser(username);

    // Iteram prin fiecare utilizator logat
    for (int i = 0; i < users.size(); i++) {
        if (users[i]->getUsername() == username)
            continue;

        // Daca este logat, ii trimitem mesajul
        std::string sender = user->getUserColor().c_str() + user->getUsername() + COLOR_DEFAULT;
        std::string reciver = users[i]->getUserColor().c_str() + users[i]->getUsername() + COLOR_DEFAULT;
        Message msg(message, sender, reciver);
        msg.setID(-1);
        users[i]->getSocket()->sendMessage(msg.toString());
    }
}