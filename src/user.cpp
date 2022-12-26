#include "user.hpp"

User::User(socklen_t server_socket)
{
    // -- Initializam structura de detalii pentru user
    this->user_details_length = sizeof(this->user_details);
    bzero(&this->user_details, this->user_details_length);

    // -- Acceptam conexiunea de la client
    this->ID = accept(server_socket, (struct sockaddr *)&user_details, (socklen_t *)&user_details_length);

    // -- Verificam daca s-a conectat clientul
    if (this->ID < 0)
    {
        std::error_code ec(this->ID, std::generic_category());
        std::cerr << "Eroare la accept: " << ec.message() << std::endl;
        exit(1);
    }

    this->logged = false;
}

// -- Functie pentru returnarea ID-ului userului
socklen_t User::getID()
{
    return this->ID;
}

// -- Functie pentru trimiterea unui mesaj catre client
void User::sendMessage(std::string message)
{
    // -- Trimitem mesajul catre client
    write(this->ID, message.c_str(), message.size());

    std::cout << "Mesajul a fost trimis catre client" << std::endl;
}

// -- Functie pentru primirea unui mesaj de la client
std::string User::getMessage()
{
    char buffer[2048];
    int read_size = read(this->ID, buffer, 2048);

    // -- Verificam daca s-a primit mesaj de la client
    if (read_size < 0)
    {
        std::error_code ec(read_size, std::generic_category());
        std::cerr << "Eroare la primirea mesajului de la client: " << ec.message() << std::endl;
        return "";
    }
    else if (read_size == 0) // -- Clientul s-a deconectat
    {
        return "";
    }

    // -- Convertim mesajul primit de la client intr-un string
    std::string message = std::string(buffer, 0, read_size);

    return message;
}

// -- Functie pentru returnarea username-ului userului
std::string User::getUsername()
{
    return this->username;
}

// -- Functie pentru setarea username-ului userului
void User::setUsername(std::string username)
{
    this->username = username;
}

// -- Functie pentru returnarea parolei userului
std::string User::getPassword()
{
    return this->password;
}

// -- Functie pentru setarea parolei userului
void User::setPassword(std::string password)
{
    this->password = password;
}

// -- Functie pentru returnarea daca user-ul este logat sau nu
bool User::isLogged()
{
    return this->logged;
}

// -- Funcie pentru setarea daca user-ul este logat sau nu
void User::setLogged(bool logged)
{
    this->logged = logged;
}