#include "src/socket.hpp"
#include <system_error>
#include <pthread.h>

// -- Portul pentru conectarea la server
int PORT = 1111;           // -- Voi folosi portul "1111" pentru serverul meu
char IP[16] = "127.0.0.1"; // -- Voi folosi IP-ul "127.0.0.1" pentru serverul meu
bool IS_USER_DISCONNECTED = false;

// -- Declararea functiilor
void sendMessageToServer(std::string message, socklen_t server_descriptor);
std::string getInputFromUser();
std::string getResponseFromServer(socklen_t server_descriptor);
static void *threadReceiveMessageFromServer(void *arg);
void showPrompt();

int main(int argc, const char *argv[])
{

    // -- Crearea unui socket pentru client
    Socket client_socket(PORT, IP);

    // -- Creeam un thread pentru a primi informatii de la server
    pthread_t thread_id;

    // -- Crearea thread-ului pentru procesarea fiecarui client in parte
    if (pthread_create(&thread_id, NULL, threadReceiveMessageFromServer, (void *)&client_socket) < 0)
    {
        std::error_code ec(client_socket.getFD(), std::generic_category());
        std::cerr << "Eroare la crearea thread-ului: " << ec.message() << std::endl;
        exit(1);
    }
    // -- Creeam o bucla infinita pentru a trimite mesaje spre serverul
    while (true)
    {
        // -- Afisam prompt-ul pentru comanda

        // -- Citim comanda de la tastatura
        std::string command = getInputFromUser();

        // -- Verificare commanda de exit
        if (command == ":exit")
        {
            IS_USER_DISCONNECTED = true;
            sendMessageToServer(":logout", client_socket.getFD());
            break;
        }
        else if (command.size() == 0)
        {
            continue;
        }
        // -- Trimitem comanda catre server
        sendMessageToServer(command, client_socket.getFD());
    }
    return 0;
}

// -- Functie pentru trimiterea mesajului catre server
void sendMessageToServer(std::string message, socklen_t server_descriptor)
{
    // -- Trimitem mesajul catre server
    send(server_descriptor, message.c_str(), message.size(), 0);
}

// -- Functie pentru citirea comenzii de la tastatura
std::string getInputFromUser()
{
    // -- Afisam prompt-ul pentru comanda
    showPrompt();
    std::string command;
    std::getline(std::cin, command);

    return command;
}

// -- Functie pentru citirea raspunsului de la server
std::string getResponseFromServer(socklen_t server_descriptor)
{
    char buffer[2048];
    int read_size = read(server_descriptor, buffer, 2048);
    buffer[read_size] = '\0';

    // -- Verificam daca s-a primit raspuns de la server
    if (IS_USER_DISCONNECTED)
    {
        std::cout << "Ati fost deconectat de la server!" << std::endl;
        exit(0);
    }
    else if (read_size < 0)
    {
        std::error_code ec(read_size, std::generic_category());
        std::cerr << "Eroare la primirea raspunsului de la server: " << ec.message() << std::endl;
        exit(1);
    }
    else if (read_size == 0)
    {
        std::cout << "Serverul a fost inchis!" << std::endl;
        exit(0);
    }

    // -- Convertim raspunsul primit de la server intr-un string
    std::string response = std::string(buffer, 0, read_size);
    return response;
}

// -- Functie pentru procesarea mesajelor primite de la server
static void *threadReceiveMessageFromServer(void *arg)
{
    // -- Castam argumentul la un socket
    Socket *client_socket = (Socket *)arg;

    // -- Creem o bucla infinita pentru a primi mesaje de la server
    while (true)
    {
        // -- Primim mesajul de la server
        std::string response = ":> " + getResponseFromServer(client_socket->getFD());
        std::cout << std::endl
                  << response << std::flush << std::endl
                  << std::endl;
        showPrompt();
    }
}

// -- Functie pentru afisarea proptului
void showPrompt()
{
    std::cout << "-> " << std::flush;
}