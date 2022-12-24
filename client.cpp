#include "src/socket.hpp"
#include <system_error>

// -- Portul pentru conectarea la server
int PORT = 1111;           // -- Voi folosi portul "1111" pentru serverul meu
char IP[16] = "127.0.0.1"; // -- Voi folosi IP-ul "127.0.0.1" pentru serverul meu

void sendMessageToServer(std::string message, socklen_t server_descriptor)
{
    // -- Trimitem mesajul catre server
    send(server_descriptor, message.c_str(), message.size(), 0);
}

// -- Functie pentru citirea comenzii de la tastatura
std::string getInputFromUser()
{
    std::string command;
    std::getline(std::cin, command);

    return command;
}

// -- Functie pentru citirea raspunsului de la server
std::string getResponseFromServer(socklen_t server_descriptor)
{
    char buffer[1024];
    int read_size = read(server_descriptor, buffer, 1024);

    // -- Verificam daca s-a primit raspuns de la server
    if (read_size < 0)
    {
        std::error_code ec(read_size, std::generic_category());
        std::cerr << "Eroare la primirea raspunsului de la server: " << ec.message() << std::endl;
        exit(1);
    }

    // -- Convertim raspunsul primit de la server intr-un string
    std::string response = std::string(buffer, 0, read_size);

    return response;
}

int main(int argc, const char *argv[])
{

    // -- Crearea unui socket pentru client
    Socket client_socket(PORT, IP);
    // -- Creeam o bucla infinita pentru a comunica cu serverul
    while (true)
    {
        // -- Afisam prompt-ul pentru comanda
        std::cout << "$ > ";

        // -- Citim comanda de la tastatura
        std::string command = getInputFromUser();

        // -- Verificare commanda de exit
        if (command == "exit")
        {
            break;
        }
        else if (command.size() == 0)
        {
            continue;
        }
        // -- Trimitem comanda catre server
        sendMessageToServer(command, client_socket.getFD());

        // -- Primim un raspuns de la server
        std::string response = getResponseFromServer(client_socket.getFD());

        // -- Afisam raspunsul primit de la server
        std::cout << response << std::endl;
    }
    return 0;
}