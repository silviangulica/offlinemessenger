#include "socket.hpp"
#include <signal.h>

// Function declarations
void readFromInput(Socket client);
void readFromServer(Socket client, pid_t childID);

int main()
{
    // Creeam un obiect client pentru a ne conecta la server
    Socket client(8080, "127.0.0.1");

    // Incepem lucrul pe 2 procese
    // 1. Pentru citire de la tastatura
    // 2. Pentru citire de la server
    pid_t child = fork();
    if (child == 0)
        readFromInput(client);      // Citeste de la tastatura si trimite la server.
    else 
        readFromServer(client, child);     // Citeste de la server si afiseaza
    return 0;
}

// Functie care citeste de la tastatura 
void readFromInput(Socket client)
{
    // Pentru a citi de la tastatura, afisam un prompt si asteptam un mesaj ceva
    while(true)
    {
        std::cout << "-> ";

        // Citim mesajul de la tastatura
        std::string message;
        std::getline(std::cin, message);

        // Verificam daca clientul a apasat "\n"
        if (message.size() == 0)
            continue;

        // Trimitem mesajul la server
        client.sendMessage(message);
    }
}

// Functi care citeste din server
void readFromServer(Socket client, pid_t childID)
{
    while(true)
    {
        // Receptionam mesajul de la server
        std::string message = client.receiveMessage();

        // Verificam daca am primit un mesaj gol (conexiun lipsa)
        if (message.size() == 0) {
            kill(childID, SIGKILL);
            exit(1);
        }

        // Afisam mesajul la output, std::flush pentru a nu pastra ce vina dupa std::endl pe buffer
        std::cout << "[]: " << message << std::endl << "-> " << std::flush;
    }
}