#include "socket.hpp"
#include "database.hpp"
#include <pthread.h>

// Declare database
Database database("database");

// Function declaration
static void* handleNewConnection(void* arg);

int main()
{
    // Cream un socket server pe portul 8080
    Socket server(8080);

    // Asteptam conexiuni
    while(true)
    {
        // Acceptam conexiunea
        Socket *client = new Socket(server.getFD());

        // Cream un nou thread pentru a se ocupa de conexiunea noua
        pthread_t thread;
        pthread_create(&thread, NULL, handleNewConnection, client);
        pthread_detach(thread);
    }

    return 0;
}

// Functie pentru a se ocupa de o noua conexiune
static void* handleNewConnection(void* arg)
{
    // Castam argumentul la Socket*
    Socket* client = (Socket*)arg;
    while(true)
    {
        std::string message = client->receiveMessage();

        // Verifiam daca am primit ceva mesaj
        if (message.size() == 0)
            break;

        client->sendMessage(message);
    }

    return NULL;
}