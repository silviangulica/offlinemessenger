#include "socket.hpp"

// -- Constructorul pentru socket de tip ascultare
Socket::Socket(int PORT)
{
    // -- Crearea socketului
    this->FD = socket(AF_INET, SOCK_STREAM, 0);

    // -- Verificam daca s-a creat socketul
    if (this->FD < 0)
    {
        std::error_code ec(this->FD, std::generic_category());
        std::cerr << "Eroare la crearea socketului: " << ec.message() << std::endl;
        exit(1);
    }

    // -- Setam structura de date pentru socket la 0
    bzero(&this->socket_details, sizeof(this->socket_details));

    // -- Incarcarea structului cu detaliile de conectare la server
    this->socket_details.sin_family = AF_INET;
    this->socket_details.sin_port = htons(PORT);
    this->socket_details.sin_addr.s_addr = INADDR_ANY;

    // -- Facem bind la socket
    if (bind(this->FD, (struct sockaddr *)&this->socket_details, sizeof(this->socket_details)) < 0)
    {
        std::error_code ec(this->FD, std::generic_category());
        std::cerr << "Eroare la bind: " << ec.message() << std::endl;
        exit(1);
    }

    // -- Ascultam pe socket, cu 5 conexiuni in asteptare
    if (listen(this->FD, 5) < 0)
    {
        std::error_code ec(this->FD, std::generic_category());
        std::cerr << "Eroare la listen: " << ec.message() << std::endl;
        exit(1);
    }
}

// -- Constructorul pentru socket de tip conectare
Socket::Socket(int PORT, char IP[16])
{
    // -- Crearea socketului
    this->FD = socket(AF_INET, SOCK_STREAM, 0);

    // -- Verificam daca s-a creat socketul
    if (this->FD < 0)
    {
        std::error_code ec(this->FD, std::generic_category());
        std::cerr << "Eroare la crearea socketului: " << ec.message() << std::endl;
        exit(1);
    }

    // -- Setam structura de date pentru socket la 0
    bzero(&this->socket_details, sizeof(this->socket_details));

    // -- Incarcarea structului cu detaliile de conectare la server
    this->socket_details.sin_family = AF_INET;
    this->socket_details.sin_port = htons(PORT);
    this->socket_details.sin_addr.s_addr = inet_addr(IP);

    // -- Facem conexiunea la server
    if (connect(this->FD, (struct sockaddr *)&this->socket_details, sizeof(this->socket_details)) < 0)
    {
        std::error_code ec(this->FD, std::generic_category());
        std::cerr << "Eroare la connect: " << ec.message() << std::endl;
        exit(1);
    }
}

// -- Functie pentru returnarea descriptorului socketului
socklen_t Socket::getFD()
{
    return this->FD;
}

// -- Deconstructorul pentru socket
Socket::~Socket()
{
    close(this->FD);
}