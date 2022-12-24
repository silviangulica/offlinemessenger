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
}

// -- Functie pentru primirea unui mesaj de la client
std::string User::getMessage()
{
    char buffer[1024];
    int read_size = read(this->ID, buffer, 1024);

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