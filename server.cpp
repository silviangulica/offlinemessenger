#include "src/socket.hpp"
#include <pthread.h>

// Function declaration
static void* handleNewConnection(void* arg);

int main()
{
    Socket server(8080);

    while(true)
    {
        Socket *client = new Socket(server.getFD());
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